add_library(jolly vm.c primitives.c)

target_include_directories(jolly PUBLIC includes)
set_target_properties(jolly PROPERTIES PUBLIC_HEADER includes/vm.h includes/primitives.h includes/memory.h)

set_target_properties(jolly PROPERTIES SOVERSION 1)

install(TARGETS jolly
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include
)
