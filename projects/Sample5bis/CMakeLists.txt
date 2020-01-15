project(Sample5bis)

if(${KIGS_PLATFORM} STREQUAL "Android" OR ${KIGS_PLATFORM} STREQUAL "WUP")
	add_library(Sample5bis SHARED "")
else()
	add_executable(Sample5bis "")
endif()

if(${KIGS_PLATFORM} STREQUAL "Windows")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SUBSYSTEM:WINDOWS /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /SUBSYSTEM:WINDOWS /DEBUG:FULL /SAFESEH:NO")
endif()

set(Sample5bis_GUID_CMAKE "A4A34A80-5863-4010-A207-8D8A094EACB9" CACHE INTERNAL "Sample5bis GUID")

target_link_libraries(Sample5bis PRIVATE DataDrivenApplication)
#target_link_libraries(Sample5bis PRIVATE Holographic)

target_include_directories(Sample5bis PRIVATE "Headers")


file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(Sample5bis 
	PRIVATE 
		${all_sources}
		${all_headers}
		)
set_property(TARGET Sample5bis PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Android/assets")

if(${KIGS_PLATFORM} STREQUAL "WUP")
	target_link_libraries(Sample5bis PRIVATE zlib)
	include_external_msproject(Sample5bis_wup_packaging "${CMAKE_CURRENT_SOURCE_DIR}/Build/UWP/Sample5bisPackaging.vcxproj")
elseif(${KIGS_PLATFORM} STREQUAL "Android")
	include_external_msproject(Sample5bis_android_packaging "${CMAKE_CURRENT_SOURCE_DIR}/Android/assoria.Sample5bis.Packaging.androidproj" TYPE "39E2626F-3545-4960-A6E8-258AD8476CE5")
	include_external_msproject(Sample5bis_kigs_java "${KIGS_PRIVATE_FRAMEWORK}/PlatformsModules/Android/Platform/Main/kigsmain/assoria.kigsmain.androidproj" TYPE "39E2626F-3545-4960-A6E8-258AD8476CE5")
elseif(${KIGS_PLATFORM} STREQUAL "Javascript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(Sample5bis PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/Android/assets@/  --js-library ${KIGS_PLATFORM_ROOT}/Platform/2DLayers/2DLayers_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/GUI/GUI_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Input/Input_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Renderer/Renderer_JavaScript.js --js-library ${KIGS_PLATFORM_ROOT}/Platform/Sound/Audio.js -s TOTAL_MEMORY=67108864 -s EXPORTED_FUNCTIONS='[_main]' --use-preload-plugins -s WASM=1 -s BINARYEN_METHOD='native-wasm' -s ALLOW_MEMORY_GROWTH=1" )
endif()
