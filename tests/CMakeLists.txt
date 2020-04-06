# If pkg-config is not installed on the system, then the CHECK_INSTALL_DIR var
# must be set to the install location of Check. For example, on Windows, this
# may be: C:/Program Files/check
# set(CHECK_INSTALL_DIR "C:/Program Files/check")

find_package(Check REQUIRED)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

set(MODEL_FILE tests)
set_directory_properties(PROPERTIES CMAKE_CONFIGURE_DEPENDS ${MODEL_FILE})
set(GENERATED_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/${MODEL_FILE})

file(REMOVE_RECURSE ${GENERATED_SOURCE_DIR})
file(MAKE_DIRECTORY ${GENERATED_SOURCE_DIR})
execute_process(COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/preprocess_tests.sh" ${CMAKE_CURRENT_SOURCE_DIR} ${GENERATED_SOURCE_DIR})

file(GLOB LIBGENERATED_FILES ${GENERATED_SOURCE_DIR}/*)

# Since Check uses Threads to paralelize the tests, it's mandatory
# add pthread as a dependency, alongside the Check libraries.
add_executable(vm_tests ${GENERATED_SOURCE_DIR}/vm_tests.c)
target_link_libraries(vm_tests jolly ${CHECK_LIBRARIES} pthread)

# Create testing target and redirect its output to `Testing` folder
add_test(NAME vm_tests COMMAND vm_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)
# set_tests_properties(vm_tests PROPERTIES TIMEOUT 30) 

# Aditional Valgrind test to check memory leaks in code
# include(FindValgrind)

# add_custom_target(valgrind_test_sample
#   COMMAND ${VALGRIND_PROGRAM} --leak-check=full $<TARGET_FILE:test_sample>
#   DEPENDS test_sample
# )