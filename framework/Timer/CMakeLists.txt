add_library(Timer STATIC "")

target_link_libraries(Timer PUBLIC BaseFramework Core)
target_include_directories(Timer PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Timer "")
else()
	GRAB_BASE_SOURCES(Timer "")
endif()
