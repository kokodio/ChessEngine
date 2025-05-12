cmake_minimum_required(VERSION 3.30)
project(chess_port)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-parentheses)

add_executable(chess_port main.cpp
        Hash_Fancy.hpp
        Perft.hpp
        Random.hpp
        Board.hpp
)