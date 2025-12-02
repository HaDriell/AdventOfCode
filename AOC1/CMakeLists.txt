cmake_minimum_required(VERSION 3.31)

project(
    AOC1
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC1 "Main.cpp")
target_compile_features(AOC1 PUBLIC cxx_std_17)