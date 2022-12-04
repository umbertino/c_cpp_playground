project (prime VERSION 1.0.0)

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

find_package(Threads REQUIRED)
find_package(GTest 1.11.0 REQUIRED)
find_package(Boost 1.78.0 COMPONENTS chrono filesystem thread REQUIRED)

# build Prime-Library
add_library(${LIB_NAME} STATIC ${SRC_DIR}/Prime.cpp)
target_link_libraries(${LIB_NAME} ${Boost_LIBRARIES})


# The main executable
add_executable(${APP_BIN_NAME} ${SRC_DIR}/main.cpp)
add_custom_target(${APP_TGT_NAME} DEPENDS ${APP_BIN_NAME})
add_dependencies(${APP_TGT_NAME} ${LIB_NAME} )
include_directories(${Boost_INCLUDE_DIRS} ${INC_DIR})
target_link_libraries(${APP_BIN_NAME} ${Boost_LIBRARIES} ${LIB_NAME})
# create a dedicated map-file for each configuration, too
#target_link_options(${APP_BIN_NAME} PRIVATE "/MAP:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/${APP_BIN_NAME}.map")

# Unit Test executable
# include(GoogleTest)
# enable_testing()
# add_executable(${TST_BIN_NAME} ${TST_SRCS})
# add_custom_target(tests DEPENDS ${TST_BIN_NAME})
# add_dependencies(${TST_BIN_NAME} prime)
# include_directories(${Boost_INCLUDE_DIRS} ${PTHREAD_INC} ${GTEST_INCLUDE_DIR} ${INC_DIR})
# target_link_libraries(${TST_BIN_NAME} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${GTEST_BOTH_LIBRARIES} prime)

# gtest_discover_tests(${TST_BIN_NAME})
