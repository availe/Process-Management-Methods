cmake_minimum_required(VERSION 3.26)
project(Process_Management_Methods)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Process_Management_Methods
        main.cpp
        sjn.cpp
        sjn.h
        otherFuncs.cpp
        otherFuncs.h)
