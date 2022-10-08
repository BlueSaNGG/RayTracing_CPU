cmake_minimum_required(VERSION 3.18)
project(cpu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")

add_executable(cpu main.cpp util.h scene.h)
