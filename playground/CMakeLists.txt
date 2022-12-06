project (playground VERSION 1.0.0)

set(APP_TGT_NAME ${PROJECT_NAME}_tgt)
set(APP_BIN_NAME ${PROJECT_NAME}_app)
set(TST_BIN_NAME ${PROJECT_NAME}_tst)
set(LIB_NAME ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost 1.78.0 COMPONENTS chrono filesystem thread REQUIRED)

# The main executable
add_executable(${APP_BIN_NAME} ${SRC_DIR}/main.cpp ${SRC_DIR}/ScopeGuard.cpp)
add_custom_target(${APP_TGT_NAME} DEPENDS ${APP_BIN_NAME})
include_directories(${Boost_INCLUDE_DIRS} ${INC_DIR})
target_link_libraries(${APP_BIN_NAME} ${Boost_LIBRARIES})

