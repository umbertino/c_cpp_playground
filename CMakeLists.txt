cmake_minimum_required (VERSION 3.22)

project (playground)

set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(TST_BIN_NAME ${PROJECT_NAME}_tst)

set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)

set(INC_DIR inc)
set(SRC_DIR src)
set(TST_DIR test)
set(LIB_DIR lib)

file(GLOB APP_INCS "${INC_DIR}/*.h" "${INC_DIR}/*.hpp")
file(GLOB APP_SRCS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB TST_SRCS "${TST_DIR}/*.cpp" "${TST_DIR}/*.c")

#set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations" )
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost 1.78.0 REQUIRED)


# build Prime-Library
add_library(libprime STATIC ${SRC_DIR}/Prime.cpp)
target_link_libraries(libprime ${Boost_LIBRARIES})

# build Scopeguard-Library
add_library(libscopeguard STATIC ${SRC_DIR}/Prime.cpp)
target_link_libraries(libscopeguard ${Boost_LIBRARIES})


# The main playground executable
add_executable(${APP_BIN_NAME} ${SRC_DIR}/playground.cpp)
add_custom_target(application DEPENDS ${APP_BIN_NAME})
add_dependencies(${APP_BIN_NAME} libprime)
include_directories(${Boost_INCLUDE_DIRS} ${INC_DIR})
target_link_libraries(${APP_BIN_NAME} ${Boost_LIBRARIES} libprime libscopeguard)


# Test Prime-Class
find_package(GTest 1.11.0 REQUIRED)
include(GoogleTest)
enable_testing()
add_executable(${TST_BIN_NAME} ${TST_SRCS})
add_custom_target(tests DEPENDS ${TST_BIN_NAME})
add_dependencies(${TST_BIN_NAME} libprime)
include_directories(${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIR} ${INC_DIR})
target_link_libraries(${TST_BIN_NAME}  ${CMAKE_THREAD_LIBS_INIT} gtest gtest_main gmock gmock_main libprime)
gtest_discover_tests(${TST_BIN_NAME})
