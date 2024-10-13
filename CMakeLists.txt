cmake_minimum_required(VERSION 3.10)
project(result-cpp VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(result-cpp
        src/main.cpp
        include/result.hpp)
