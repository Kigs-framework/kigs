add_library(CoreAnimation STATIC "")

target_link_libraries(CoreAnimation PUBLIC BaseFramework Core SceneGraph Renderer TinyImage Input Timer FileManager XML)

target_include_directories(CoreAnimation PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(CoreAnimation "")
else()
	GRAB_BASE_SOURCES(CoreAnimation "")
endif()
