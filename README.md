# Spelet C++

A C++ game using BearLibTerminal for display and OpenAL for audio.

## Prerequisites

Before building, ensure you have the following dependencies installed:

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake g++ pkg-config libopenal-dev
```

### BearLibTerminal
Download and install BearLibTerminal from:
https://github.com/cfyzium/bearlibterminal/releases

For Linux, copy the library files:
```bash
sudo cp libBearLibTerminal.so /usr/local/lib/
sudo cp BearLibTerminal.h /usr/local/include/
sudo ldconfig
```

### macOS
```bash
brew install cmake openal-soft
```

### Windows
Install OpenAL SDK and BearLibTerminal, then configure CMake to find them.

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Running

```bash
./Spelet
```

## Controls

- **WASD / Arrow Keys**: Move player
- **Space**: Action (plays sound if audio is available)
- **ESC**: Quit game

## Project Structure

```
├── CMakeLists.txt      # Build configuration
├── src/
│   ├── main.cpp        # Main game entry point and game loop
│   ├── audio.hpp       # Audio manager header
│   └── audio.cpp       # Audio manager implementation (OpenAL)
└── README.md           # This file
```

## Features

- BearLibTerminal-based terminal rendering
- OpenAL audio system for sound effects
- Simple game loop with input handling
- Frame rate limiting (60 FPS)
- Basic player movement
