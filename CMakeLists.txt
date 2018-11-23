cmake_minimum_required(VERSION 3.12)
project(c C)

set(CMAKE_C_STANDARD 11)

add_executable(c
        ex3/calculator.c ex3/stack.c ex3/calculatorUtils.c)
