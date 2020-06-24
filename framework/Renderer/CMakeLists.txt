add_library(Renderer STATIC "")

target_link_libraries(Renderer PUBLIC SceneGraph Input GUI TinyImage freetype)

if(USE_D3D)
	set(PLATFORM_MODULE "RendererDirectX11")
else()
	set(PLATFORM_MODULE "RendererOpenGL3")

	if(${KIGS_PLATFORM} STREQUAL "WUP")
		target_link_libraries(Renderer PUBLIC angle)
	elseif(${KIGS_PLATFORM} STREQUAL "Windows")
		target_link_libraries(Renderer PRIVATE glew)
	endif()
endif()

target_include_directories(Renderer 
	PUBLIC 
		"Headers"
		"${PLATFORM_MODULE}/Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Renderer ${PLATFORM_MODULE})
else()
	
	set(PLATFORM_EXCLUDED "")
	GRAB_BASE_SOURCES(Renderer "${PLATFORM_EXCLUDED}")

	
	set(EXCLUDED
			".*GLSLDeferred\\.cpp"
			".*GLSLPostProcess\\.cpp")
	
	GRAB_PLATFORM_SOURCES(Renderer ${PLATFORM_MODULE} "${EXCLUDED}")

	target_sources(Renderer 
		PRIVATE 
			"../StandAloneUtilityClasses/Sources/ModernMeshBuilder.cpp"
			"../StandAloneUtilityClasses/Sources/simplexnoise.cpp"
			"../StandAloneUtilityClasses/Sources/BinMeshLoaderBase.cpp"
			"../StandAloneUtilityClasses/Sources/GLSLDebugDraw.cpp"
			"../StandAloneUtilityClasses/Sources/FreeType_TextDrawer.cpp" )
endif()
