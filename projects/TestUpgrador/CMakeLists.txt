project(TestUpgrador)

add_executable(TestUpgrador "")
set(TestUpgrador_GUID_CMAKE "3A92D16F-3319-40CF-85C9-CC6D83EDBCCC" CACHE INTERNAL "TestUpgrador GUID")

if(${KIGS_PLATFORM} STREQUAL "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SUBSYSTEM:CONSOLE /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SUBSYSTEM:CONSOLE /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /SUBSYSTEM:CONSOLE /DEBUG:FULL /SAFESEH:NO")
endif()

target_link_libraries(TestUpgrador PRIVATE BaseApplication)
target_include_directories(TestUpgrador PRIVATE "Headers")

file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(TestUpgrador 
	PRIVATE 
		${all_sources}
		${all_headers}
		)

set_property(TARGET TestUpgrador PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(TestUpgrador PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/ -s TOTAL_MEMORY=67108864 -s EXPORTED_FUNCTIONS='[_main]' --use-preload-plugins -s WASM=1 -s BINARYEN_METHOD='native-wasm' -s ALLOW_MEMORY_GROWTH=1" )
endif()
