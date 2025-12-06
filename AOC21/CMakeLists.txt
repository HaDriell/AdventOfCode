cmake_minimum_required(VERSION 3.31)

project(
    AOC21
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC21 "Main.cpp")
target_compile_features(AOC21 PUBLIC cxx_std_17)