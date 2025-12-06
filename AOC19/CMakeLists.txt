cmake_minimum_required(VERSION 3.31)

project(
    AOC19
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC19 "Main.cpp")
target_compile_features(AOC19 PUBLIC cxx_std_17)