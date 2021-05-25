add_library(TinyImage STATIC "")

target_link_libraries(TinyImage PUBLIC Core)
target_link_libraries(TinyImage PRIVATE png jpeg zlib zstdlib)

if(${KIGS_PLATFORM} STREQUAL "Windows")
	target_link_libraries(TinyImage PRIVATE gif)
endif()

target_include_directories(TinyImage PUBLIC "Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(TinyImage "")
else()
	GRAB_BASE_SOURCES(TinyImage "")
endif()
