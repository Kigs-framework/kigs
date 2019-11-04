add_library(GUI STATIC "")

target_link_libraries(GUI PUBLIC SceneGraph Renderer Input)
target_include_directories(GUI PUBLIC "Headers")

set(PLATFORM_MODULE "GUI${KIGS_PLATFORM}")

target_include_directories(GUI 
	PRIVATE 
		"${PLATFORM_MODULE}/Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(GUI ${PLATFORM_MODULE})
else()
	GRAB_BASE_SOURCES(GUI "")
	GRAB_PLATFORM_SOURCES(GUI ${PLATFORM_MODULE} "")
endif()
