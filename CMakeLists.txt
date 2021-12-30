cmake_minimum_required (VERSION 3.22)

project (playground)

set(incdir ./inc)
set(srcdir ./src)
file(GLOB all_srcs "${srcdir}/*.cpp" "${srcdir}/*.c")
file(GLOB all_hdrs "${incdir}/*.h" "${incdir}/*.hpp")

include_directories(${incdir})

add_executable(playground ${all_srcs})
