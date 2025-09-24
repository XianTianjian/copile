cmake_minimum_required(VERSION 3.10)  # CMake 最低版本要求
project(CompilerProject LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 将源文件列出来
set(SOURCES
    Compiler.cpp
    Lexer.cpp
)

set(HEADERS
    Lexer.h
    Token.h
)

# 生成可执行文件
add_executable(Compiler ${SOURCES} ${HEADERS})
