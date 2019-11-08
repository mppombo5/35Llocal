cmake_minimum_required(VERSION 3.15)
project(sfrobu C)

set(CMAKE_C_STANDARD 11)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address")

add_executable(sfrobu sfrobu.c)