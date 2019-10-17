cmake_minimum_required(VERSION 3.15)
project(escapeRoom C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -pthread)

add_executable(escapeRoom main.c objects.h queue.h)