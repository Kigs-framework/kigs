project(KigsDocs)

add_executable(KigsDocs "")
set(KigsDocs_GUID_CMAKE "F89F41EF-2DE4-49FE-9315-501C92CD4ADC" CACHE INTERNAL "KigsDocs GUID")

if(${KIGS_PLATFORM} STREQUAL "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SUBSYSTEM:CONSOLE /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SUBSYSTEM:CONSOLE /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /SUBSYSTEM:CONSOLE /DEBUG:FULL /SAFESEH:NO")
endif()

target_link_libraries(KigsDocs PRIVATE BaseApplication 2DLayers Renderer LuaKigsBind CoreAnimation Collision HTTPRequest Thread GenericAnimation)
target_include_directories(KigsDocs PRIVATE "Headers")

file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(KigsDocs 
	PRIVATE 
		${all_sources}
		${all_headers}
		)

set_property(TARGET KigsDocs PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(KigsDocs PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ -s TOTAL_MEMORY=67108864 -s EXPORTED_FUNCTIONS='[_main]' --use-preload-plugins -s WASM=1 -s BINARYEN_METHOD='native-wasm' -s ALLOW_MEMORY_GROWTH=1" )
endif()
