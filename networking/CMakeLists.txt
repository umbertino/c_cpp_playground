project (asio_net VERSION 1.0.0)

set(APP_TGT_NAME ${PROJECT_NAME}_tgt)
set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(LIB_NAME ${PROJECT_NAME})
set(INC_DIR inc)
set(SRC_DIR src)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)
find_package(asio REQUIRED)
# find_package(Boost 1.78.0 COMPONENTS asio REQUIRED)


# The main executable
add_executable(${APP_BIN_NAME} ${SRC_DIR}/main.cpp)
target_include_directories(${APP_BIN_NAME} PRIVATE ${ASIO_DIR} ${INC_DIR})
target_link_libraries(${APP_BIN_NAME} Threads::Threads)
