# 2048 Game in C++

A console-based implementation of the classic 2048 game with customizable board size.

## Features

- 🎮 Classic 2048 gameplay mechanics
- 🔢 Adjustable board size (3x3 to 9x9)
- 📊 Score tracking
- 🏆 Maximum tile achievement tracking
- 🖥️ Cross-platform support (Windows/Linux/macOS)
- 🎨 Dynamic board formatting based on tile values

## Requirements

- C++ compiler (GCC, Clang, or MSVC)
- CMake (optional, for building)

## How to Play

### Controls
- W   - Move Up
- A  - Move Left
- S  - Move Down
- D  - Move Right
- Q - Quit Game

### Game Rules
1. Tiles slide in the chosen direction until they hit the wall or another tile
2. Matching tiles merge into their sum
3. After each move, a new tile (2 or 4) appears
4. Game ends when the board is full with no possible merges

## Installation & Running

### Compile and Run
```bash
g++ -o 2048 game.cpp
./2048
