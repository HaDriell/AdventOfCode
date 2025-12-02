cmake_minimum_required(VERSION 3.31)

project(
    AOC3
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC3 "Main.cpp")
target_compile_features(AOC3 PUBLIC cxx_std_17)