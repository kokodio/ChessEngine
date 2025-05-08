cmake_minimum_required(VERSION 3.30)
project(chess_port)

set(CMAKE_CXX_STANDARD 23)

add_executable(chess_port main.cpp
        Search.hpp
        Hash_Fancy.hpp
        Perft.hpp
        Random.hpp
        Console.hpp
        Board.hpp
)
