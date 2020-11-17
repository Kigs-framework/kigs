add_library(TinyImage STATIC "")

target_link_libraries(TinyImage PUBLIC Core)

if(NOT ${KIGS_PLATFORM} STREQUAL "Javascript")
	target_link_libraries(TinyImage PRIVATE png jpeg gif zlib zstdlib)
else()
	target_link_libraries(TinyImage PRIVATE zstdlib)
endif()

target_include_directories(TinyImage PUBLIC "Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(TinyImage "")
else()
	GRAB_BASE_SOURCES(TinyImage "")
endif()
