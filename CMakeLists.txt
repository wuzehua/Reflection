cmake_minimum_required(VERSION 3.13)
project(Reflection)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(SOURCE 
    ${SOURCE}
    src/main.cpp
)

add_executable(demo ${SOURCE})