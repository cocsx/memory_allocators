cmake_minimum_required(VERSION 3.25)
project(ArenaAllocatorTest CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Adding DEBUG macro
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")

    # Disable compiler optimizations for profiling
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif()

find_package(GTest REQUIRED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${TRACY})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        GTest::GTest
        ArenaAllocator)

add_test(ArenaAllocatorTests ${PROJECT_NAME})
