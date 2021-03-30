project(libs)

# imgui
message(STATUS "add imgui")
add_subdirectory("cross_platform/ImGui")

# jpeg
message(STATUS "add jpeg")
add_subdirectory("cross_platform/jpeg")
SET_TARGET_PROPERTIES( jpeg   					PROPERTIES  FOLDER "libs" )

# png
message(STATUS "add png")
add_subdirectory("cross_platform/png")
SET_TARGET_PROPERTIES( png   					PROPERTIES  FOLDER "libs" )

# tiff
message(STATUS "add Tiff")
add_subdirectory("cross_platform/tiff")
SET_TARGET_PROPERTIES( tiff   					PROPERTIES  FOLDER "libs" )

# gif
message(STATUS "add gif")
add_subdirectory("cross_platform/gif")
SET_TARGET_PROPERTIES( gif   					PROPERTIES  FOLDER "libs" )

# zlib
message(STATUS "add zlib")
add_subdirectory("cross_platform/zlib")
SET_TARGET_PROPERTIES( zlib   					PROPERTIES  FOLDER "libs" )

# lua
message(STATUS "add lua")
add_subdirectory("cross_platform/lua")
SET_TARGET_PROPERTIES( lua   					PROPERTIES  FOLDER "libs" )

# freetype
message(STATUS "add freetype")
add_subdirectory("cross_platform/freetype/custom_cmake")
SET_TARGET_PROPERTIES( freetype   					PROPERTIES  FOLDER "libs" )

# zstd
message(STATUS "add zstd")
if(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(ZSTD_MULTITHREAD_SUPPORT OFF CACHE BOOL "Set ZSTD Multithread off" FORCE)
	set(ZSTD_BUILD_SHARED OFF CACHE BOOL "Set ZSTD shared off" FORCE)
endif()
add_subdirectory("cross_platform/zstd/build/cmake")
add_subdirectory("cross_platform/zstd/custom_cmake")

# concurrentqueue
message(STATUS "add concurrentqueue")
add_subdirectory("cross_platform/concurrentqueue")

if( ("${KIGS_PLATFORM}" STREQUAL "Windows") OR ("${KIGS_PLATFORM}" STREQUAL "WUP") )
	set(WinDir "win32")
	if(KIGS_PLATFORM_X64)
		set(WinDir "win64")
	endif()

	# glew 
	message(STATUS "add glew")
	add_subdirectory("${WinDir}/glew")

endif()

# miniz
message(STATUS "add miniz")
add_subdirectory("cross_platform/miniz")
SET_TARGET_PROPERTIES( miniz   					PROPERTIES  FOLDER "libs" )


if( ("${KIGS_PLATFORM}" STREQUAL "Windows") OR ("${KIGS_PLATFORM}" STREQUAL "WUP") )
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/win32/fbxsdk/lib")
	message(STATUS "add fbxsdk")
	add_subdirectory("win32/fbxsdk")
	set(FBXSDK_AVAILABLE ON CACHE BOOL "fbx sdk is available")
	endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cross_platform/box2d/src")
	message(STATUS "add box2d")
	set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "no BOX2D_BUILD_UNIT_TESTS")
	set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "no BOX2D_BUILD_TESTBED")
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "no BUILD_SHARED_LIBS" FORCE)
	add_subdirectory("cross_platform/box2d")
	SET_TARGET_PROPERTIES(box2d   					PROPERTIES  FOLDER "libs" )
endif()

