cmake_minimum_required(VERSION 3.31)

project(
    AOC22
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC22 "Main.cpp")
target_compile_features(AOC22 PUBLIC cxx_std_17)