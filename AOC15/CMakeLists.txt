cmake_minimum_required(VERSION 3.31)

project(
    AOC15
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC15 "Main.cpp")
target_compile_features(AOC15 PUBLIC cxx_std_17)