SimpleNES
=============
An NES emulator written in C++ (now with audio!)

Roughly 50-60% of games should work (ie. games that use either no mapper or mappers 1, 2, 3 and experimental support for 4, 7, 66 and 11).

---

**ARCHIVED**: I no longer have the capacity or motivation to entertain new features/issues/PRs. Feel free to fork and/or use the current build.  

---



Examples of games that have been tested to run (but NOT limited to):

(USA/Japan or World versions only i.e. NTSC compatible)

* Super Mario Bros.
* Contra
* Adventure Island
* Ninja Gaiden
* Wrecking Crew
* Megaman and Megaman 2
* Mario Bros.
* Donky Kong and Donkey Kong Jr.
* Battle City
* Paperboy
* Legend of Zelda
* Pacman
* Tennis
* Excitebike
* Nightmare Elm Street
* Cabal
* Battletoads
* Arch Rivals
* etc...


Screenshots
------------------------
![Screenshot 1](http://amhndu.github.io/screenshots/nes1.png)
![Screenshot 2](http://amhndu.github.io/screenshots/nes2.png)
![Screenshot 3](http://amhndu.github.io/screenshots/nes3.png)
![Screenshot 4](http://amhndu.github.io/screenshots/nes4.png)
![Screenshot 5](http://amhndu.github.io/screenshots/nes5.png)
![Screenshot 6](http://amhndu.github.io/screenshots/nes6.png)
![Screenshot 6](http://amhndu.github.io/screenshots/nes7.png)
![Screenshot 6](http://amhndu.github.io/screenshots/nes8.png)

Videos
------------
**OUTDATED**(missing audio)  [YouTube Playlist](https://www.youtube.com/playlist?list=PLiULt7qySWt2VbHTkvIt9kYPMPcWt01qN)


Compiling
-----------

You need:
* SFML 3 development headers and libraries (tested with 3.0.2)
* A C++17 compiler
* CMake 3.22 or newer

On macOS, the build automatically discovers an existing Homebrew `sfml` installation:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

For an installation outside the standard search paths, pass its prefix:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/path/to/sfml
cmake --build build --parallel
```

You can also set `SFML_DIR` to the directory containing `SFMLConfig.cmake`.
Audio uses the bundled miniaudio library; SFML's Audio component is not required.
`-DBUILD_STATIC=ON` requires an SFML installation with static libraries.

See [.github/workflows/compile.yml](.github/workflows/compile.yml) for Linux, macOS and Windows builds.

Installing SFML
-----------------
* macOS: `brew install sfml` (skip this if already installed).
* Windows: `vcpkg install sfml:x64-windows`, then configure CMake with the vcpkg toolchain.
* Linux/FreeBSD: install SFML 3 from your package manager, or [build SFML from source](https://www.sfml-dev.org/tutorials/3.0/getting-started/build-from-source/). Older distributions may still package SFML 2, which is incompatible.


Running
-----------------

Just pass the path to a .nes image like

```
$ ./SimpleNES ~/Games/SuperMarioBros.nes
```
To set size of the window,
```
$ ./SimpleNES -w 600 ~/Games/Contra.nes
```
For supported command line options, try
```
$ ./SimpleNES -h
SimpleNES is a simple NES emulator.
It can run off .nes images.
Set keybindings with keybindings.conf

Usage: SimpleNES [options] rom-path

Options:
-h, --help             Print this help text and exit
--mute-audio           Mute audio
-s, --scale            Set video scale. Default: 3.
                       Scale of 1 corresponds to 256x240
-w, --width            Set the width of the emulation screen (height is
                       set automatically to fit the aspect ratio)
-H, --height           Set the height of the emulation screen (width is
                       set automatically to fit the aspect ratio)
                       This option is mutually exclusive to --width
-C, --conf             Set the keybindings file's path. The default 
                       keybindings file is keybindings.conf.

```

Controller
-----------------

Keybindings can be configured with keybindings.conf. SFML 3 key names are used; existing names such as `Return`, `BackSpace`, and `Quote` remain accepted.


Default keybindings:

**Player 1**

 Button        | Mapped to
 --------------|-------------
 Start         | Return/Enter
 Select        | Right Shift
 A             | J
 B             | K
 Up            | W
 Down          | S
 Left          | A
 Right         | D


**Player 2**

 Button        | Mapped to
 --------------|-------------
 Start         | Numpad9
 Select        | Numpad8
 A             | Numpad5
 B             | Numpad6
 Up            | Up
 Down          | Down
 Left          | Left
 Right         | Right

