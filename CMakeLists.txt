cmake_minimum_required(VERSION 3.25)
project(Tape)

set(CMAKE_CXX_STANDARD 23)
add_executable(Tape main.cpp tape.cpp tape.h)

add_subdirectory(my_tests)

enable_testing()

