add_library(XML STATIC "")

target_link_libraries(XML PUBLIC BaseFramework Core)

target_include_directories(XML PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(XML "")
else()
	GRAB_BASE_SOURCES(XML "")
endif()
