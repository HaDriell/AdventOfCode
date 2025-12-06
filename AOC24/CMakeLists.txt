cmake_minimum_required(VERSION 3.31)

project(
    AOC24
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC24 "Main.cpp")
target_compile_features(AOC24 PUBLIC cxx_std_17)