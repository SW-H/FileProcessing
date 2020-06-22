cmake_minimum_required(VERSION 3.16)
project(FileStructure_HW6 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(FileStructure_HW6
        heapsort.c
        person.h)
