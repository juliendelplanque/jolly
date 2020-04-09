# I used this https://github.com/vndmtrx/check-cmake-example/blob/master/tests/CMakeLists.txt
# as a basis to build this cmake config.
add_subdirectory(lib)

add_executable(main jolly.c)
target_link_libraries(main jolly)
