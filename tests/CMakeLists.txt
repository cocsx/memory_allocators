cmake_minimum_required(VERSION 3.25)
project(memory_allocators_tests LANGUAGES CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Adding DEBUG macro
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -DDEBUG")

    # Disable compiler optimizations for profiling
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()

add_compile_options(-Wall -Wextra -pedantic -Werror)


find_package(GTest REQUIRED)


file(GLOB_RECURSE SOURCES "allocators_tests.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        memory_allocators
        GTest::GTest)

gtest_discover_tests(${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ctest --output-on-failure)
