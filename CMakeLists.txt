cmake_minimum_required (VERSION 3.22)

project (playground)

set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(TST_BIN_NAME ${PROJECT_NAME}_tst)

set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)

set(incdir inc)
set(srcdir src)
set(tstdir test)

file(GLOB app_srcs "${srcdir}/*.cpp" "${srcdir}/*.c")
file(GLOB tst_srcs "${tstdir}/*.cpp" "${tstdir}/*.c")
file(GLOB app_hdrs "${incdir}/*.h" "${incdir}/*.hpp")

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations -DGTEST_IS_THREADSAFE" )
set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.78.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} ${incdir})
#target_link_libraries(playground ${Boost_LIBRARIES})
add_executable(${APP_BIN_NAME} ${app_srcs})

find_package(GTest REQUIRED)
enable_testing()

#TestPrime-Class
include_directories(${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIR} ${incdir})
add_executable(Prime_${TST_BIN_NAME} ${tst_srcs})
target_link_libraries(Prime_${TST_BIN_NAME} gtest gtest_main)
add_test(Prime_${TST_BIN_NAME} Prime_${TST_BIN_NAME} )


