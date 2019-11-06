add_library(2DLayers STATIC "")

target_link_libraries(2DLayers PUBLIC TinyImage Input Renderer SceneGraph)

target_include_directories(2DLayers PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(2DLayers "")
else()
	GRAB_BASE_SOURCES(2DLayers "")
endif()
