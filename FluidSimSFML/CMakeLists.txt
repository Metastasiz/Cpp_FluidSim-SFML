cmake_minimum_required(VERSION 3.14)
project(untitled)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Fluid.h)
add_executable(untitled ${SOURCE_FILES})
include_directories(/usr/local/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(untitled sfml-system sfml-window sfml-graphics sfml-audio sfml-network)