cmake_minimum_required(VERSION 3.31)

project(
    AOC18
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(AOC18 "Main.cpp")
target_compile_features(AOC18 PUBLIC cxx_std_17)