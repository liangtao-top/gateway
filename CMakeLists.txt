cmake_minimum_required(VERSION 3.22)

project(GW C)

set(CMAKE_C_STANDARD 23)

add_executable(GW src/main.c src/struct/thread_pool_t.h src/struct/thread_pool_task_t.h src/pool/manager.c src/pool/manager.h src/utils/helper.h src/http/server.c src/http/server.h)

add_executable(pool src/pool.c src/struct/thread_pool_t.h src/struct/thread_pool_task_t.h src/pool/manager.c src/pool/manager.h src/utils/helper.h src/http/server.c src/http/server.h)

find_package(Threads)

target_link_libraries(GW ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(pool ${CMAKE_THREAD_LIBS_INIT})