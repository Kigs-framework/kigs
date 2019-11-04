add_library(FileManager STATIC "")

target_link_libraries(FileManager PUBLIC BaseFramework Core)

target_include_directories(FileManager PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(FileManager "")
else()
	GRAB_BASE_SOURCES(FileManager "")
endif()
