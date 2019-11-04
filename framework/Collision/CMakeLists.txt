add_library(Collision STATIC "")

target_link_libraries(Collision PUBLIC SceneGraph Renderer concurrentqueue)

target_include_directories(Collision PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Collision "")
else()
	GRAB_BASE_SOURCES(Collision "")
endif()
