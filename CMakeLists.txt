cmake_minimum_required(VERSION 3.20)

project(codecoach_super LANGUAGES CXX)

# Permite C++20 en ambos subproyectos
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agregar subproyectos
add_subdirectory(lib_codecoach)
add_subdirectory(gui_codecoach)























