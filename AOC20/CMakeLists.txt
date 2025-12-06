cmake_minimum_required(VERSION 3.31)

project(
    AOC20
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC20 "Main.cpp")
target_compile_features(AOC20 PUBLIC cxx_std_17)