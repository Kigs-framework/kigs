add_library(ImGui STATIC "")

target_link_libraries(ImGui PUBLIC 2DLayers SceneGraph Renderer Input)
target_include_directories(ImGui PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(ImGui "")
else()
	GRAB_BASE_SOURCES(ImGui "")
	target_link_libraries(ImGui PRIVATE dearimgui_sources)
endif()
