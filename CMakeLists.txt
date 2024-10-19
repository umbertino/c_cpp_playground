cmake_minimum_required (VERSION 3.18)

project (playground VERSION 1.0.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/asio/include)

message("Building with CMake version: ${CMAKE_VERSION}")

#add_subdirectory(prime)
add_subdirectory(networking)
