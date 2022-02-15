cmake_minimum_required (VERSION 3.22)

project (playground)

set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(TST_BIN_NAME ${PROJECT_NAME}_tst)

set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)

set(INC_DIR inc)
set(SRC_DIR src)
set(TST_DIR test)

file(GLOB APP_INCS "${INC_DIR}/*.h" "${INC_DIR}/*.hpp")
file(GLOB APP_SRCS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB TST_SRCS "${TST_DIR}/*.cpp" "${TST_DIR}/*.c")

#set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations" )
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost 1.78.0 REQUIRED)


add_executable(${APP_BIN_NAME} ${APP_SRCS})
add_custom_target(application DEPENDS ${APP_BIN_NAME})
include_directories(${Boost_INCLUDE_DIRS} ${INC_DIR})
target_link_libraries(${APP_BIN_NAME} ${Boost_LIBRARIES})


# TestPrime-Class
find_package(GTest 1.11.0 REQUIRED)
include(GoogleTest)
enable_testing()
add_executable(Prime_${TST_BIN_NAME} ${TST_SRCS})
add_custom_target(tests DEPENDS Prime_${TST_BIN_NAME})
include_directories(${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIR} ${INC_DIR})
target_link_libraries(Prime_${TST_BIN_NAME}  ${CMAKE_THREAD_LIBS_INIT} gtest gtest_main gmock gmock_main)
gtest_discover_tests(Prime_${TST_BIN_NAME})
