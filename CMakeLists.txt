cmake_minimum_required(VERSION 3.30)
project(chess_port)

set(CMAKE_CXX_STANDARD 23)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-parentheses)
else()
    add_compile_options(/W4 /permissive-)
endif()

add_executable(chess_port main.cpp
        Hash_Fancy.hpp
        Perft.hpp
        Random.hpp
        Board.hpp
)