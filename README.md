# Sculpt-3D

An open-source, cross platform, 3D sculpting application.

This is a Qt6-based desktop application with a Vulkan viewport, which uses the [Vulkan metal extension](https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VK_EXT_metal_objects.html) on macOS (not [MoltenVK](https://github.com/KhronosGroup/MoltenVK)) and has been tested on:

- macOS Sequoia
- Ubuntu 24.04 LTS
- Windows 11

## Linux

This code has been developed and tested on Ubuntu 24.04 LTS, which required the following packages:

- git
- build-essential
- cmake
- ninja-build
- clang-format
- libxcb-cursor0
- libxcb-cursor-dev
- qt6
- libglu1-mesa-dev
- freeglut3-dev
- mesa-common-dev
- vulkan-sdk
- libfontconfig1-dev
- libfreetype-dev
- libx11-dev
- libx11-xcb-dev
- libxcb-cursor-dev
- libxcb-glx0-dev
- libxcb-icccm4-dev
- libxcb-image0-dev
- libxcb-keysyms1-dev
- libxcb-randr0-dev
- libxcb-render-util0-dev
- libxcb-shape0-dev
- libxcb-shm0-dev
- libxcb-sync-dev
- libxcb-util-dev
- libxcb-xfixes0-dev
- libxcb-xinerama0-dev
- libxcb-xkb-dev
- libxcb1-dev
- libxext-dev
- libxfixes-dev
- libxi-dev
- libxkbcommon-dev
- libxkbcommon-x11-dev
- libxrender-dev
