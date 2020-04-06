add_subdirectory(lib)

add_executable(main jolly.c)
target_link_libraries(main jolly)
