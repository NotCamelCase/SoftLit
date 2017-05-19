cmake_minimum_required (VERSION 3.1)

project(SoftLit)

find_package(OpenMP)

if (OPENMP_FOUND)
    message("FOUND")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${OpenMP_C_FLAGS}  ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_library(SDL2_LIBRARY SDL2)

add_executable(SoftLit Display.h
                       Display.cpp
                       Primitive.cpp
                       Primitive.h
                       RasterizerConfig.h
                       Rasterizer.cpp
                       Rasterizer.h
                       Texture.h
                       Texture.cpp
                       Shaders.h
                       SoftLit.cpp
                       stdafx.cpp
                       stdafx.h
                       Utils.h)

# glm           -> https://github.com/g-truc/glm.git
# tinyobjloader -> https://github.com/syoyo/tinyobjloader.git
# stb           -> https://github.com/nothings/stb.git 

target_include_directories(SoftLit PRIVATE ${SDL2_INCLUDE_DIR}
                                           deps/glm
                                           deps/tinyobjloader
                                           deps/stb)

target_link_libraries(SoftLit ${SDL2_LIBRARY})

set_target_properties(SoftLit PROPERTIES CXX_STANDARD          14
                                         CXX_STANDARD_REQUIRED ON)