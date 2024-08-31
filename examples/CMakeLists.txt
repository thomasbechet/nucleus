cmake_minimum_required(VERSION 3.5.0)

add_subdirectory(${CMAKE_SOURCE_DIR}/nucleus/external/glfw-3.4 glfw)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(graphics)
add_subdirectory(basic)
add_subdirectory(cxx)
