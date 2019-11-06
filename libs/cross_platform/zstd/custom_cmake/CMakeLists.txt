
add_library(zstdlib INTERFACE)

target_include_directories(zstdlib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/")

target_link_libraries(zstdlib INTERFACE libzstd_static)

SET_TARGET_PROPERTIES( libzstd_static PROPERTIES  FOLDER "libs" )
