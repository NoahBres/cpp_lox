cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "cpp_lox")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

project(${PROJECT_NAME})

file(GLOB SOURCES "src/*.cpp")

add_executable(cpp_lox ${SOURCES})
