# Zombie Knight (rpg04)
Advanced C++ project from Team 04

This project is a 2D RPG built in C++ utilizing an Entity Component System (ECS). It is cross-platform compatible (Linux/Windows 64-bit), uses CMake for the build system, `vcpkg` (via git submodules) for dependency management and ClangFormat for code styling.

## Prerequisites

Before building, ensure your system has the following installed:
* **Git** (for cloning and submodules)
* **CMake** (v3.15 or higher recommended)
* **C++ Compiler** supporting C++17/C++20 (GCC/Clang for Linux, MSVC for Windows)

## Step 1: Cloning the Repository

Because this project relies on `vcpkg` and `Tileson` as submodules, you must clone the repository recursively to fetch the dependencies:

```bash
git clone --recursive <URL>
cd rpg04/
```
