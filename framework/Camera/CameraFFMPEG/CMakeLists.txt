add_library(CameraFFMPEG STATIC "")
message(STATUS "add CameraFFMPEG")
target_link_libraries(CameraFFMPEG PUBLIC BaseFramework Camera 2DLayers ffmpeg)

target_include_directories(CameraFFMPEG PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(CameraFFMPEG "")
else()
	GRAB_BASE_SOURCES(CameraFFMPEG "")
endif()