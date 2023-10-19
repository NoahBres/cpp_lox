cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER /opt/homebrew/Cellar/llvm/16.0.6/bin/clang)
set(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm/16.0.6/bin/clang++)

set(PROJECT_NAME "cpp_lox")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

project(${PROJECT_NAME})

file(GLOB SOURCES "src/*.cpp")

add_executable(cpp_lox ${SOURCES})
