cmake_minimum_required(VERSION 3.31)

project(
    AOC10
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC10 "Main.cpp")
target_compile_features(AOC10 PUBLIC cxx_std_17)