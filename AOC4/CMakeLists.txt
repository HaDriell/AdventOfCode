cmake_minimum_required(VERSION 3.31)

project(
    AOC4
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC4 "Main.cpp")
target_compile_features(AOC4 PUBLIC cxx_std_17)