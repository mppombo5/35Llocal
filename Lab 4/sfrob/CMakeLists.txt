cmake_minimum_required(VERSION 3.15)
project(sfrob C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -fsanitize=address)

add_executable(sfrob sfrob.c)