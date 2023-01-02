cmake_minimum_required(VERSION 3.25)
project(memory_allocators C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compile_commands.json

add_subdirectory(src)
add_subdirectory(tests)
