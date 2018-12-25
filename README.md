# SoftLit

Software Rasterizer

![Test2](http://imgur.com/F1Mp2L8.gif)

# Building
1) Update git submodules if deps/ not fetched when cloning repo
  - git submodule update --init --recursive
2) Install SDL2 packages for your OS/distro (On non-MS systems)
3) On Windows, CMake needs fix for including/dynamic linking with SDL2 so manually compile and link with SDL2 and point to SDL2.dll
4) Create and compile the project with CMake (thanks to [@mtezych](https://github.com/mtezych))
5) Put assets/ in the parent directory of binary
6) Get mesmerized by the beautiful imagery that's being rendered! :-D

# Awesome Third Party Libraries 
- [SDL](https://www.libsdl.org/)
- [GLM](http://glm.g-truc.net/0.9.8/index.html)
- [tinyobjloader](http://syoyo.github.io/tinyobjloader/)
- [stb_image](https://github.com/nothings/stb)