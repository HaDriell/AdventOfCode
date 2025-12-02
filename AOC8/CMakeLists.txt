cmake_minimum_required(VERSION 3.31)

project(
    AOC8
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC8 "Main.cpp")
target_compile_features(AOC8 PUBLIC cxx_std_17)