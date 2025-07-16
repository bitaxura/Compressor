cmake_minimum_required(VERSION 3.10.0)
project(Compressor VERSION 0.1.0 LANGUAGES CXX)

add_executable(Compressor huffman.cpp main.cpp utils.cpp)

set_target_properties(Compressor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")