cmake_minimum_required(VERSION 3.31)

project(
    AOC2
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC2 "Main.cpp")
target_compile_features(AOC2 PUBLIC cxx_std_17)