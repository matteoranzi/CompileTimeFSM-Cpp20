cmake_minimum_required(VERSION 3.26.5)
project(CompileTimeFSM-Cpp20)

set(CMAKE_CXX_STANDARD 20)

add_executable(CompileTimeFSM-Cpp20 example/main.cpp)

target_include_directories(CompileTimeFSM-Cpp20 PUBLIC FSM)
