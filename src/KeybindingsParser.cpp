#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Controller.h"
#include "Log.h"

namespace sn
{
// trim from start (construct new string)
inline std::string ltrim(const std::string& str)
{
    std::string s(str);
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); }));
    return s;
}

// trim from end (construct new string)
inline std::string rtrim(const std::string& str)
{
    std::string s(str);
    s.erase(std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) { return std::isspace(c); }).base(), s.end());
    return s;
}

void parseControllerConf(std::string filepath, std::vector<sf::Keyboard::Key>& p1, std::vector<sf::Keyboard::Key>& p2)
{
    const std::string buttonStrings[]               = { "A", "B", "Select", "Start", "Up", "Down", "Left", "Right" };
    using Key                                       = sf::Keyboard::Key;
    // Map names explicitly so bindings do not depend on SFML's enum ordering.
    const std::unordered_map<std::string, Key> keys = {
        { "A", Key::A },
        { "B", Key::B },
        { "C", Key::C },
        { "D", Key::D },
        { "E", Key::E },
        { "F", Key::F },
        { "G", Key::G },
        { "H", Key::H },
        { "I", Key::I },
        { "J", Key::J },
        { "K", Key::K },
        { "L", Key::L },
        { "M", Key::M },
        { "N", Key::N },
        { "O", Key::O },
        { "P", Key::P },
        { "Q", Key::Q },
        { "R", Key::R },
        { "S", Key::S },
        { "T", Key::T },
        { "U", Key::U },
        { "V", Key::V },
        { "W", Key::W },
        { "X", Key::X },
        { "Y", Key::Y },
        { "Z", Key::Z },
        { "Num0", Key::Num0 },
        { "Num1", Key::Num1 },
        { "Num2", Key::Num2 },
        { "Num3", Key::Num3 },
        { "Num4", Key::Num4 },
        { "Num5", Key::Num5 },
        { "Num6", Key::Num6 },
        { "Num7", Key::Num7 },
        { "Num8", Key::Num8 },
        { "Num9", Key::Num9 },
        { "Escape", Key::Escape },
        { "LControl", Key::LControl },
        { "LShift", Key::LShift },
        { "LAlt", Key::LAlt },
        { "LSystem", Key::LSystem },
        { "RControl", Key::RControl },
        { "RShift", Key::RShift },
        { "RAlt", Key::RAlt },
        { "RSystem", Key::RSystem },
        { "Menu", Key::Menu },
        { "LBracket", Key::LBracket },
        { "RBracket", Key::RBracket },
        { "Semicolon", Key::Semicolon },
        { "Comma", Key::Comma },
        { "Period", Key::Period },
        { "Apostrophe", Key::Apostrophe },
        { "Slash", Key::Slash },
        { "Backslash", Key::Backslash },
        { "Grave", Key::Grave },
        { "Equal", Key::Equal },
        { "Hyphen", Key::Hyphen },
        { "Space", Key::Space },
        { "Enter", Key::Enter },
        { "Backspace", Key::Backspace },
        { "Tab", Key::Tab },
        { "PageUp", Key::PageUp },
        { "PageDown", Key::PageDown },
        { "End", Key::End },
        { "Home", Key::Home },
        { "Insert", Key::Insert },
        { "Delete", Key::Delete },
        { "Add", Key::Add },
        { "Subtract", Key::Subtract },
        { "Multiply", Key::Multiply },
        { "Divide", Key::Divide },
        { "Left", Key::Left },
        { "Right", Key::Right },
        { "Up", Key::Up },
        { "Down", Key::Down },
        { "Numpad0", Key::Numpad0 },
        { "Numpad1", Key::Numpad1 },
        { "Numpad2", Key::Numpad2 },
        { "Numpad3", Key::Numpad3 },
        { "Numpad4", Key::Numpad4 },
        { "Numpad5", Key::Numpad5 },
        { "Numpad6", Key::Numpad6 },
        { "Numpad7", Key::Numpad7 },
        { "Numpad8", Key::Numpad8 },
        { "Numpad9", Key::Numpad9 },
        { "F1", Key::F1 },
        { "F2", Key::F2 },
        { "F3", Key::F3 },
        { "F4", Key::F4 },
        { "F5", Key::F5 },
        { "F6", Key::F6 },
        { "F7", Key::F7 },
        { "F8", Key::F8 },
        { "F9", Key::F9 },
        { "F10", Key::F10 },
        { "F11", Key::F11 },
        { "F12", Key::F12 },
        { "F13", Key::F13 },
        { "F14", Key::F14 },
        { "F15", Key::F15 },
        { "Pause", Key::Pause },
        // Accept existing SFML 2 configuration files.
        { "SemiColon", Key::Semicolon },
        { "Quote", Key::Apostrophe },
        { "BackSlash", Key::Backslash },
        { "Tilde", Key::Grave },
        { "Dash", Key::Hyphen },
        { "Return", Key::Enter },
        { "BackSpace", Key::Backspace },
    };

    std::ifstream file(filepath);
    std::string   line;
    enum
    {
        Player1,
        Player2,
        None
    } state              = None;
    unsigned int line_no = 0;
    while (std::getline(file, line))
    {
        ++line_no;
        line = rtrim(ltrim(line));
        if (line.empty() || line[0] == '#')
            continue;
        else if (line == "[Player1]")
        {
            state = Player1;
        }
        else if (line == "[Player2]")
        {
            state = Player2;
        }
        else if (state == Player1 || state == Player2)
        {
            const auto divider = line.find('=');
            if (divider == std::string::npos)
            {
                LOG(Error) << "Missing '=' in configuration file at Line " << line_no << std::endl;
                continue;
            }
            const auto button  = rtrim(ltrim(line.substr(0, divider)));
            const auto keyName = rtrim(ltrim(line.substr(divider + 1)));
            const auto it      = std::find(std::begin(buttonStrings), std::end(buttonStrings), button);
            const auto key     = keys.find(keyName);
            if (it == std::end(buttonStrings) || key == keys.end())
            {
                LOG(Error) << "Invalid key in configuration file at Line " << line_no << std::endl;
                continue;
            }
            const auto i                    = std::distance(std::begin(buttonStrings), it);
            (state == Player1 ? p1 : p2)[i] = key->second;
        }
        else
            LOG(Error) << "Invalid line in key configuration at Line " << line_no << std::endl;
    }
}
}
