cmake_minimum_required (VERSION 3.18)

project (playground VERSION 1.0.0)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

message("Building with CMake version: ${CMAKE_VERSION}")

add_subdirectory(prime)
