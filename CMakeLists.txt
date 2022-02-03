cmake_minimum_required (VERSION 3.22)

project (playground)

set(incdir ./inc)
set(srcdir ./src)
file(GLOB all_srcs "${srcdir}/*.cpp" "${srcdir}/*.c")
file(GLOB all_hdrs "${incdir}/*.h" "${incdir}/*.hpp")

set(CMAKE_CXX_FLAGS "-Wno-deprecated-declarations")

find_package(Boost 1.74.0 REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS} ${incdir})
    #target_link_libraries(playground ${Boost_LIBRARIES})
    add_executable(playground ${all_srcs})
endif()
