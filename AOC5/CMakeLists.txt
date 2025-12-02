cmake_minimum_required(VERSION 3.31)

project(
    AOC5
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC5 "Main.cpp")
target_compile_features(AOC5 PUBLIC cxx_std_17)