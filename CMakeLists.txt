

cmake_minimum_required(VERSION 3.20)
project(CodeCoach)

# Si tu lib usa su propio CMakeLists:
add_subdirectory(lib_codecoach)

# Nuestra GUI Qt
add_subdirectory(gui_codecoach)
