add_library(glew32s STATIC IMPORTED)

set_property(TARGET glew32s PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/glew32s.lib")

add_library(glew INTERFACE)
target_link_libraries(glew INTERFACE glew32s opengl32 glu32)
target_include_directories(glew INTERFACE "include")
target_compile_definitions(glew INTERFACE GLEW_STATIC)
