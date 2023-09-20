cmake_minimum_required(VERSION 2.8)
project(func)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DEXPORT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -lpthread ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -g -Wl,-rpath=./lib:../lib ")

include_directories(
        ${PROJECT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_executable(func ${SOURCE_FILES} )

target_link_libraries(func pthread)
