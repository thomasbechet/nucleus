cmake_minimum_required(VERSION 3.5.0)

project(tests LANGUAGES C VERSION 0.0.1)

include_directories(../)

set(CMAKE_BUILD_TYPE Debug)

add_executable(vec vec.c)

add_test(vec vec)
