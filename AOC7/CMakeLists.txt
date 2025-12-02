cmake_minimum_required(VERSION 3.31)

project(
    AOC7
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC7 "Main.cpp")
target_compile_features(AOC7 PUBLIC cxx_std_17)