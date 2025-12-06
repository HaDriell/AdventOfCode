cmake_minimum_required(VERSION 3.31)

project(
    AOC16
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC16 "Main.cpp")
target_compile_features(AOC16 PUBLIC cxx_std_17)