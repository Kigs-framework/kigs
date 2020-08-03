project(tools)

if(${KIGS_PLATFORM} STREQUAL "Windows")
add_subdirectory(BundleList)
add_subdirectory(MeshConvert)
add_subdirectory(KigsPackager)
add_subdirectory(TextureCompress)
add_subdirectory(KigsDocs)
endif()