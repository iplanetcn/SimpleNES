#include "Emulator.h"
#include "APU/Constants.h"
#include "Log.h"

#include <chrono>

namespace sn
{
using std::chrono::high_resolution_clock;

Emulator::Emulator()
  : m_cpu(m_bus)
  , m_audioPlayer(static_cast<int>(1.0 / apu_clock_period_s.count()))
  , m_ppu(m_pictureBus, m_emulatorScreen)
  , m_apu(m_audioPlayer, m_cpu.createIRQHandler(), [&](Address addr) { return DMCDMA(addr); })
  , m_bus(m_ppu, m_apu, m_controller1, m_controller2, [&](Byte b) { OAMDMA(b); })
  , m_screenScale(3.f)
  , m_lastWakeup()
{
    m_ppu.setInterruptCallback([&]() { m_cpu.nmiInterrupt(); });
}

void Emulator::run(std::string rom_path)
{
    if (!m_cartridge.loadFromFile(rom_path))
        return;

    m_mapper = Mapper::createMapper(static_cast<Mapper::Type>(m_cartridge.getMapper()),
                                    m_cartridge,
                                    m_cpu.createIRQHandler(),
                                    [&]() { m_pictureBus.updateMirroring(); });
    if (!m_mapper)
    {
        LOG(Error) << "Creating Mapper failed. Probably unsupported." << std::endl;
        return;
    }

    if (!m_bus.setMapper(m_mapper.get()) || !m_pictureBus.setMapper(m_mapper.get()))
    {
        return;
    }

    m_cpu.reset();
    m_ppu.reset();

    // SFML 3 constructs a graphics context with RenderWindow, even before create().
    // Keep command-line help and ROM validation independent of a display.
    auto& window = m_window.emplace();
    window.create(sf::VideoMode({ static_cast<unsigned int>(NESVideoWidth * m_screenScale),
                                  static_cast<unsigned int>(NESVideoHeight * m_screenScale) }),
                  "SimpleNES",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);
    m_emulatorScreen.create(NESVideoWidth, NESVideoHeight, m_screenScale, sf::Color::White);

    m_lastWakeup  = high_resolution_clock::now();
    m_elapsedTime = m_lastWakeup - m_lastWakeup;

    m_audioPlayer.start();

    bool focus = true, pause = false;
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            const auto* pressed  = event->getIf<sf::Event::KeyPressed>();
            const auto* released = event->getIf<sf::Event::KeyReleased>();
            if (event->is<sf::Event::Closed>() || (pressed && pressed->code == sf::Keyboard::Key::Escape))
            {
                window.close();
                return;
            }
            else if (event->is<sf::Event::FocusGained>())
            {
                focus          = true;
                const auto now = high_resolution_clock::now();
                LOG(Info) << "Gained focus. Removing " << (now - m_lastWakeup).count() << "ns from timers" << std::endl;
                m_lastWakeup = now;
            }
            else if (event->is<sf::Event::FocusLost>())
            {
                focus = false;
                LOG(Info) << "Losing focus; paused." << std::endl;
            }
            else if (pressed && pressed->code == sf::Keyboard::Key::F2)
            {
                pause = !pause;
                if (!pause)
                {
                    const auto now = high_resolution_clock::now();
                    LOG(Info) << "Unpaused. Removing " << (now - m_lastWakeup).count() << "ns from timers" << std::endl;
                    m_lastWakeup = now;
                }
                else
                {
                    LOG(Info) << "Paused." << std::endl;
                }
            }
            else if (pause && released && released->code == sf::Keyboard::Key::F3)
            {
                for (int i = 0; i < 29781; ++i) // Around one frame
                {
                    // PPU
                    m_ppu.step();
                    m_ppu.step();
                    m_ppu.step();
                    // CPU
                    m_cpu.step();
                    // APU
                    m_apu.step();
                }
            }
            else if (focus && released && released->code == sf::Keyboard::Key::F4)
            {
                Log::get().setLevel(Info);
            }
            else if (focus && released && released->code == sf::Keyboard::Key::F5)
            {
                Log::get().setLevel(InfoVerbose);
            }
        }

        if (focus && !pause)
        {
            const auto now  = high_resolution_clock::now();
            m_elapsedTime  += now - m_lastWakeup;
            m_lastWakeup    = now;

            while (m_elapsedTime > cpu_clock_period_ns)
            {
                // PPU
                m_ppu.step();
                m_ppu.step();
                m_ppu.step();
                // CPU
                m_cpu.step();
                // APU
                m_apu.step();

                m_elapsedTime -= cpu_clock_period_ns;
            }

            window.draw(m_emulatorScreen);
            window.display();
        }
        else
        {
            sf::sleep(sf::milliseconds(1000 / 60));
        }
    }
}

void Emulator::OAMDMA(Byte page)
{
    m_cpu.skipOAMDMACycles();
    auto page_ptr = m_bus.getPagePtr(page);
    if (page_ptr != nullptr)
    {
        m_ppu.doDMA(page_ptr);
    }
    else
    {
        LOG(Error) << "Can't get pageptr for DMA" << std::endl;
    }
}

Byte Emulator::DMCDMA(Address addr)
{
    m_cpu.skipDMCDMACycles();
    return m_bus.read(addr);
};

void Emulator::setVideoHeight(int height)
{
    m_screenScale = height / float(NESVideoHeight);
    LOG(Info) << "Scale: " << m_screenScale << " set. Screen: " << int(NESVideoWidth * m_screenScale) << "x"
              << int(NESVideoHeight * m_screenScale) << std::endl;
}

void Emulator::setVideoWidth(int width)
{
    m_screenScale = width / float(NESVideoWidth);
    LOG(Info) << "Scale: " << m_screenScale << " set. Screen: " << int(NESVideoWidth * m_screenScale) << "x"
              << int(NESVideoHeight * m_screenScale) << std::endl;
}
void Emulator::setVideoScale(float scale)
{
    m_screenScale = scale;
    LOG(Info) << "Scale: " << m_screenScale << " set. Screen: " << int(NESVideoWidth * m_screenScale) << "x"
              << int(NESVideoHeight * m_screenScale) << std::endl;
}

void Emulator::setKeys(std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2)
{
    m_controller1.setKeyBindings(p1);
    m_controller2.setKeyBindings(p2);
}

void Emulator::muteAudio()
{
    m_audioPlayer.mute();
}

}
