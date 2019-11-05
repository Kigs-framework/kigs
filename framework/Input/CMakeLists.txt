add_library(Input STATIC "")

target_link_libraries(Input PUBLIC GUI)
target_include_directories(Input PUBLIC "Headers")

set(PLATFORM_MODULE "Input${KIGS_PLATFORM}")

target_include_directories(Input 
	PRIVATE 
		"${PLATFORM_MODULE}/Headers")

if(${KIGS_PLATFORM} STREQUAL "Windows")
	target_link_libraries(Input PUBLIC dinput8 dxguid strmiids)
endif()


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Input ${PLATFORM_MODULE})
else()
	GRAB_BASE_SOURCES(Input "")
	GRAB_PLATFORM_SOURCES(Input ${PLATFORM_MODULE} ".*KeyBoardVirtualJoystickDX\\.cpp")
endif()
