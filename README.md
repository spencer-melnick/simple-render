# simple-render

A simple Vulkan renderer I'm working on to gain more experience with low-level graphics programming.

## Requirements

### All Operating Systems

- Software
    - [CMake](https://cmake.org)
    - [Vulkan SDK](https://vulkan.lunarg.com/)
- Libraries
    - [SDL2](https://www.libsdl.org/)
    - [fmt](https://github.com/fmtlib/fmt)
    - [spdlog](https://github.com/gabime/spdlog)

### Windows
- [vcpkg (Recommended)](https://github.com/microsoft/vcpkg)
    - vcpkg is an open source package manager for Windows, and allows for installation of required libraries like on a Linux ecosystem. If you are using vcpkg and set the environment variable `VCPKG_ROOT`, CMake will be able to automatically find (but not install) all needed libraries

## Build Instructions

### Windows

- Install CMake and the Vulkan SDK
- (Optional) Install vcpkg according to the instruction on its [Github page](https://github.com/microsoft/vcpkg)
- (Optional) set your `VCPKG_ROOT` directory
- Install the required libraries
    - If you've set up vcpkg, you can grab all the libraries by running `.\vcpkg install sdl2[vulkan] vulkan fmt spdlog` in your vcpkg folder
- Configure and build
    - Currently I am using the [CMake Tools](https://github.com/microsoft/vscode-cmake-tools) extension for [VSCode](https://code.visualstudio.com/), but any IDE or modern compiler should work

## Code Standards

For my own reference:

- General
    - All code should be commented thorougly. If you aren't sure if it's immediately obvious what a chunk of code does, add a comment!
    - Use descriptive names for variables, functions, and classes; limit abbreviations
    - Use whitespace to group related lines of code and make source code easier to parse later
    - Class names should use PascalCase, functions and variables should use camelCase
    - Prefix member variables with `m_`
    - Limit function length. If a function takes a few hundred lines, consider breaking it up into separate functions
    - Use lowercase file names with dashes