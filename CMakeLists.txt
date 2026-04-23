cmake_minimum_required(VERSION 3.16)
project(ray-tui LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE all_sources src/main/*.cpp src/main/*.hpp)

set(shared_sources ${all_sources})
list(FILTER shared_sources EXCLUDE REGEX ".*/main\\.cpp$")

add_executable(cli ${all_sources})
target_include_directories(cli PUBLIC src/main)

enable_testing()

file(GLOB test_sources test/*.cpp)
foreach(test_src ${test_sources})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src} ${shared_sources})
    target_include_directories(${test_name} PUBLIC src/main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
