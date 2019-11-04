add_library(GenericAnimation STATIC "")

target_link_libraries(GenericAnimation PUBLIC SceneGraph Renderer)

target_include_directories(GenericAnimation PUBLIC "Headers" "Headers/Bones")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(GenericAnimation "")
else()
	GRAB_BASE_SOURCES(GenericAnimation "")
endif()
