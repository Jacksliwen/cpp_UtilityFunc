cmake_minimum_required(VERSION 2.8)
project(func)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEXPORT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wl,-rpath=./lib:../lib")

file(GLOB_RECURSE SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/func.cpp
)

add_executable(func main.cpp func.cpp )

