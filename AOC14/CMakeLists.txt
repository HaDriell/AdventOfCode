cmake_minimum_required(VERSION 3.31)

project(
    AOC14
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC14 "Main.cpp")
target_compile_features(AOC14 PUBLIC cxx_std_17)