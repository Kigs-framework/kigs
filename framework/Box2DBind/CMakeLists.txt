if(TARGET box2d)

	message(STATUS "box2d found")

	add_library(Box2DBind STATIC "")

	target_link_libraries(Box2DBind PUBLIC 2DLayers box2d)

	target_include_directories(Box2DBind PUBLIC "Headers")

	if(UNITY_BUILD)
		GRAB_UNITY_BUILD_SOURCES(Box2DBind "")
	else()
		GRAB_BASE_SOURCES(Box2DBind "")
	endif()
endif()
