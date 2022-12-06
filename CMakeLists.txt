# Main CMaleList.txt
cmake_minimum_required (VERSION 3.22)

project (top VERSION 1.0.0)

# set Release and Debug for multi Config build
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Standard folder names
set(INC_DIR inc)
set(SRC_DIR src)
set(TST_DIR tst)
set(LIB_DIR lib)

# set binary and library folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

# File globbing
file(GLOB APP_INCS "${INC_DIR}/*.h" "${INC_DIR}/*.hpp" "${INC_DIR}/*.tpp")
file(GLOB APP_SRCS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB TST_SRCS "${TST_DIR}/*.cpp" "${TST_DIR}/*.c")

# Google-Test Preparation
set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)

# include the sub-projects
add_subdirectory(playground)
add_subdirectory(prime)
add_subdirectory(customErrorCodes)
