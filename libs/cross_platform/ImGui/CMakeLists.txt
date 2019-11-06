add_library(dearimgui INTERFACE)
target_include_directories(dearimgui INTERFACE "ExternCrossPlatformLibraries/ImGui/")

add_library(dearimgui_sources INTERFACE)
target_sources(dearimgui_sources 
	INTERFACE 
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/lz4/lz4.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/lz4/xxhash.c"
		)