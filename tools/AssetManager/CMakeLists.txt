project(AssetManager)

add_executable(AssetManager "")
set(AssetManager_GUID_CMAKE "263C16F4-F471-4C99-93E1-A3AD65F4C06C" CACHE INTERNAL "AssetManager GUID")

if(${KIGS_PLATFORM} STREQUAL "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SUBSYSTEM:CONSOLE /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SUBSYSTEM:CONSOLE /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /SUBSYSTEM:CONSOLE /DEBUG:FULL /SAFESEH:NO")
endif()

target_link_libraries(AssetManager PRIVATE BaseApplication Thread)
target_include_directories(AssetManager PRIVATE "Headers")

file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(AssetManager 
	PRIVATE 
		${all_sources}
		${all_headers}
		)

set_property(TARGET AssetManager PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(AssetManager PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ -s TOTAL_MEMORY=67108864 -s EXPORTED_FUNCTIONS='[_main]' --use-preload-plugins -s WASM=1 -s BINARYEN_METHOD='native-wasm' -s ALLOW_MEMORY_GROWTH=1" )
endif()
