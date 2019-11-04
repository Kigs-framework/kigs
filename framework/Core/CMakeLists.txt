add_library(Core STATIC "")

target_link_libraries(Core PUBLIC BaseFramework XML FileManager Timer)
target_include_directories(Core PUBLIC "Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Core "")
else()
	GRAB_BASE_SOURCES(Core "")
	target_sources(Core 
		PRIVATE 
			"${KIGS_PLATFORM_ROOT}/Platform/Core/CorePlatform.cpp"
			"${KIGS_PLATFORM_ROOT}/Platform/Core/PlatformBaseApplication.cpp")
endif()
