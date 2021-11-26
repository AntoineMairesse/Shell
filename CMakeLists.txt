cmake_minimum_required(VERSION 3.20)
project(shell C)

set(CMAKE_C_STANDARD 99)

file(GLOB SOURCES "*.c")
add_executable(shell ${SOURCES})

