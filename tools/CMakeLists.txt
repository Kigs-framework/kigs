project(tools)

if(${KIGS_PLATFORM} STREQUAL "Windows")
add_subdirectory(BundleList)
add_subdirectory(MeshConvert)
endif()