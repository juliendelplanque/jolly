cmake_minimum_required(VERSION 3.15)

project(jolly)
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} \
    -g \
    -Wall -Werror -pedantic")

add_subdirectory(src)
