cmake_minimum_required(VERSION 3.15)

project(jolly)

# Custom cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Configure wether libraries will be static or shared linked
set(BUILD_SHARED_LIBS OFF)

# Enable debugging
set(CMAKE_BUILD_TYPE Debug)

# Enable Coverage Tests
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -O0")

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} \
    -g \
    -Wall -Werror -pedantic")

# Enable coloring of logs.
set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} \
    -DLOG_USE_COLOR")

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)