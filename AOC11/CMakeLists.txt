cmake_minimum_required(VERSION 3.31)

project(
    AOC11
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC11 "Main.cpp")
target_compile_features(AOC11 PUBLIC cxx_std_17)