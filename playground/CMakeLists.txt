project (playground VERSION 1.0.0)

set(APP_TGT_NAME ${PROJECT_NAME}_tgt)
set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(TST_BIN_NAME ${PROJECT_NAME}_tst)
set(LIB_NAME ${PROJECT_NAME})

set(GTEST_INCLUDE_DIR $ENV{GTEST_ROOT}/include)
set(GTEST_LIBRARY_DIR $ENV{GTEST_ROOT}/lib)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

set(INC_DIR inc)
set(SRC_DIR src)
set(TST_DIR tst)
set(LIB_DIR lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)

file(GLOB APP_INCS "${INC_DIR}/*.h" "${INC_DIR}/*.hpp" "${INC_DIR}/*.tpp")
file(GLOB APP_SRCS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.c")
file(GLOB TST_SRCS "${TST_DIR}/*.cpp" "${TST_DIR}/*.c")

set(CMAKE_CXX_STANDARD 17)

# find_package(Threads REQUIRED)
# find_package(Boost 1.78.0 COMPONENTS chrono filesystem thread REQUIRED)

# The main executable
add_executable(${APP_BIN_NAME} ${SRC_DIR}/main.cpp)
add_custom_target(${APP_TGT_NAME} DEPENDS ${APP_BIN_NAME})
include_directories(${Boost_INCLUDE_DIRS} ${INC_DIR})
# create a dedicated map-file for each configuration, too
#target_link_options(${APP_BIN_NAME} PRIVATE "/MAP:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/${APP_BIN_NAME}.map")

