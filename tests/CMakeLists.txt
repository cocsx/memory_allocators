cmake_minimum_required(VERSION 3.25)
project(memory_allocators_tests C)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Adding DEBUG macro
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")

    # Disable compiler optimizations for profiling
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -pedantic -Werror)

file(GLOB_RECURSE SOURCES "*.c")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        memory_allocators)

add_test(ArenaAllocatorTests ${PROJECT_NAME})
