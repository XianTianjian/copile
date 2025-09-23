cmake_minimum_required(VERSION 3.10)
project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Compiler
    Compiler.cpp
    Lexer.cpp
)
