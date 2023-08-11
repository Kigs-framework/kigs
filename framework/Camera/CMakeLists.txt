
add_library(Camera STATIC "")

target_link_libraries(Camera PUBLIC BaseFramework Core TinyImage Input)

target_include_directories(Camera PUBLIC "Headers")

set(PLATFORM_MODULE "Camera${KIGS_PLATFORM}")

target_include_directories(Camera PRIVATE "${PLATFORM_MODULE}/Headers"
										  "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Camera ${PLATFORM_MODULE})
else()
	GRAB_BASE_SOURCES(Camera "")
	GRAB_PLATFORM_SOURCES(Camera ${PLATFORM_MODULE} "")
endif()

if("${KIGS_PLATFORM}" STREQUAL "Windows")
	add_subdirectory(CameraVLC)
endif()
