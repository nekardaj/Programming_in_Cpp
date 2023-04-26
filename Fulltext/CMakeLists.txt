cmake_minimum_required(VERSION 3.20)
project(Fulltext)

set(CMAKE_CXX_STANDARD 17)

add_executable(Fulltext main.cpp TextProcessors.h TextProcessors.cpp)
