cmake_minimum_required(VERSION 3.27)
project(docking C)

set(CMAKE_C_STANDARD 11)

add_executable(docking main.c
        primary.c
        primary.h
        kernel.c
        kernel.h
)
