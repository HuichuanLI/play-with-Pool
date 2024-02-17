cmake_minimum_required(VERSION 3.25)
project(play_with_thread_pool)

set(CMAKE_CXX_STANDARD 17)


add_executable(play_with_reactor   nginx_mem_pool.cpp nginx_mem_pool.h main.cpp)

