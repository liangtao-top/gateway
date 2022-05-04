#设置cmake最低版本
cmake_minimum_required(VERSION 3.22)
#cmake工程的名称。
project(Gateway C)
#定义变量。
set(CMAKE_C_STANDARD 23)
set(CMAKE_BUILD_TYPE Debug)
#使用指定的源文件来生成目标可执行文件。
add_executable(Gateway main.c)


file(GLOB_RECURSE GATEWAY_SOURCES "src/c/*.c")
file(GLOB_RECURSE GATEWAY_HEADERS "src/c/*.h")

add_library(library STATIC ${GATEWAY_HEADERS} ${GATEWAY_SOURCES})

target_link_libraries(Gateway library)



