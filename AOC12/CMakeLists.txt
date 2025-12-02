cmake_minimum_required(VERSION 3.31)

project(
    AOC12
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC12 "Main.cpp")
target_compile_features(AOC12 PUBLIC cxx_std_17)