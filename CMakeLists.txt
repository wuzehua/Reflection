cmake_minimum_required(VERSION 3.13)
project(reflection)

option(BUILD_REFLECTION_TESTS "Build tests" ON)

set(CMAKE_CXX_STANDARD 20)

if(BUILD_REFLECTION_TESTS)
    add_subdirectory(test)
endif()