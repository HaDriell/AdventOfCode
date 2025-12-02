cmake_minimum_required(VERSION 3.31)

project(
    AOC9
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC9 "Main.cpp")
target_compile_features(AOC9 PUBLIC cxx_std_17)