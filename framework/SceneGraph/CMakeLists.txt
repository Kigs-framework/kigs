add_library(SceneGraph STATIC "")

target_link_libraries(SceneGraph PUBLIC Input Renderer concurrentqueue)

target_include_directories(SceneGraph PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(SceneGraph "")
else()
	GRAB_BASE_SOURCES(SceneGraph "")
endif()
