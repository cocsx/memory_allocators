cmake_minimum_required(VERSION 3.16)
project(memory_allocators C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Export compile_commands.json

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
