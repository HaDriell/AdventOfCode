cmake_minimum_required(VERSION 3.31)

project(
    AOC6
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC6 "Main.cpp")
target_compile_features(AOC6 PUBLIC cxx_std_17)