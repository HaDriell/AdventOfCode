cmake_minimum_required(VERSION 3.31)

project(
    AOC13
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC13 "Main.cpp")
target_compile_features(AOC13 PUBLIC cxx_std_17)