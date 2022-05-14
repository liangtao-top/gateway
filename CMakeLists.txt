cmake_minimum_required(VERSION 3.22)

project(GW C)

set(CMAKE_C_STANDARD 23)

add_executable(GW src/main.c)

find_package(Threads)

target_link_libraries(GW ${CMAKE_THREAD_LIBS_INIT})