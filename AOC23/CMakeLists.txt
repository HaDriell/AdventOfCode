cmake_minimum_required(VERSION 3.31)

project(
    AOC23
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC23 "Main.cpp")
target_compile_features(AOC23 PUBLIC cxx_std_17)