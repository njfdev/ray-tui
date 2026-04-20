cmake_minimum_required(VERSION 3.16)
project(ray-tui LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE sources src/main/*.cpp src/main/*.h)

add_executable(cli ${sources})
target_include_directories(cli PUBLIC src/main)
