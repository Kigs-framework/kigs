project(Sample8)

if(${KIGS_PLATFORM} STREQUAL "Android" OR ${KIGS_PLATFORM} STREQUAL "WUP")
	add_library(Sample8 SHARED "")
else()
	add_executable(Sample8 "")
endif()

if(${KIGS_PLATFORM} STREQUAL "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SUBSYSTEM:WINDOWS /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /SUBSYSTEM:WINDOWS /DEBUG:FULL /SAFESEH:NO")
endif()

set(Sample8_GUID_CMAKE "1B841500-17A9-4A1F-B57B-F7231B7760C3" CACHE INTERNAL "Sample8 GUID")

target_link_libraries(Sample8 PRIVATE DataDrivenApplication)
#target_link_libraries(Sample8 PRIVATE Holographic)

target_include_directories(Sample8 PRIVATE "Headers")


file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(Sample8 
	PRIVATE 
		${all_sources}
		${all_headers}
		)
set_property(TARGET Sample8 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

#add custom prebuild asset generation
if(${KIGS_PLATFORM} STREQUAL "Android")
add_custom_command(TARGET Sample8  PRE_BUILD 
										 COMMAND Xcopy \"${CMAKE_CURRENT_SOURCE_DIR}/assets\" \"${CMAKE_CURRENT_SOURCE_DIR}/Sample8Android/app/src/main/assets\" /e /s /y
										 )

endif()											 

if(${KIGS_PLATFORM} STREQUAL "WUP")
	target_link_libraries(Sample8 PRIVATE zlib)
	include_external_msproject(Sample8_wup_packaging "${CMAKE_CURRENT_SOURCE_DIR}/Build/UWP/Sample8Packaging.vcxproj")
elseif(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(Sample8 PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/  --js-library ${KIGS_PLATFORM_ROOT}/Platform/2DLayers/2DLayers_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/GUI/GUI_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Input/Input_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Renderer/Renderer_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Sound/Audio.js -s TOTAL_MEMORY=67108864 -s EXPORTED_FUNCTIONS='[_main]' --use-preload-plugins -s WASM=1 -s BINARYEN_METHOD='native-wasm' -s ALLOW_MEMORY_GROWTH=1" )
endif()
