cmake_minimum_required (VERSION 3.22)

set(TST_BIN_NAME ${PROJECT_NAME}_tst)

set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)

set(INC_DIR inc)
set(SRC_DIR src)
set(TST_DIR test)
set(LIB_DIR lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

file(GLOB APP_INCS "${INC_DIR}/*.h" "${INC_DIR}/*.hpp")
file(GLOB APP_SRCS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB TST_SRCS "${TST_DIR}/*.cpp" "${TST_DIR}/*.c")

#set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations" )
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost 1.78.0 REQUIRED)

# build Prime-Library
add_library(prime STATIC ${SRC_DIR}/Prime.cpp)
target_link_libraries(prime ${Boost_LIBRARIES})

# build Scopeguard-Library
add_library(scopeguard STATIC ${SRC_DIR}/Scopeguard.cpp)
target_link_libraries(scopeguard ${Boost_LIBRARIES})

# Test Prime-Class
find_package(GTest 1.11.0 REQUIRED)
include(GoogleTest)
enable_testing()
add_executable(${TST_BIN_NAME} ${TST_SRCS})
add_custom_target(tests DEPENDS ${TST_BIN_NAME})
add_dependencies(${TST_BIN_NAME} prime)
include_directories(${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIR} ${INC_DIR})
target_link_libraries(${TST_BIN_NAME}  ${CMAKE_THREAD_LIBS_INIT} gtest gtest_main gmock gmock_main prime)
gtest_discover_tests(${TST_BIN_NAME})
