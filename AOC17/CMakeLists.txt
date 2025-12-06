cmake_minimum_required(VERSION 3.31)

project(
    AOC17
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC17 "Main.cpp")
target_compile_features(AOC17 PUBLIC cxx_std_17)