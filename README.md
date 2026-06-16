# Zombie Knight (rpg04)
Advanced C++ project from Team 04

This project is a 2D RPG built in C++ utilizing an Entity Component System (ECS). It is cross-platform compatible (Linux/Windows 64-bit), uses CMake for the build system, `vcpkg` (via git submodules) for dependency management and ClangFormat for code styling.

## Prerequisites

Before building, ensure your system has the following installed:
* **Git** (for cloning and submodules)
* **CMake** (CMake v3.31 or newer is recommended when building with the included vcpkg setup)
* **C++ Compiler** supporting C++17/C++20 (GCC/Clang for Linux, MSVC for Windows)

### For Linux (Ubuntu)

```bash
sudo apt-get update
sudo apt-get install -y \ git \ cmake \ build-essential \ pkg-config \ curl \ zip \ unzip \ tar \ ca-certificates \ libx11-dev \ libxi-dev \ libxrandr-dev \ libxcursor-dev \ libxinerama-dev \ libudev-dev \ libgl1-mesa-dev \ libglu1-mesa-dev \ libopenal-dev \ libvorbis-dev \ libflac-dev \ libfreetype-dev
```

## Step 1: Cloning the Repository
This project uses Git submodules for dependencies such as vcpkg and Tileson. Therefore, clone the repository including submodules:

```bash
git clone https://github.com/ACPP-Team04/rpg04.git
cd rpg04/
sudo apt-get install pkg-config
git submodule update --init --recursive
sudo apt-get install libx11-dev libxi-dev libxrandr-dev libxcursor-dev libudev-dev libgl1-mesa-dev make
```

## Step 2: Configure the Project
```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=./vcpkg/scripts/buildsystems/vcpkg.cmake
```

## Step 3: Build the Project
```bash
cmake --build build
```

## Step 4: Run game
```bash
./build/main
```

## Step 5: Run tests
```bash
./build/tests
```
