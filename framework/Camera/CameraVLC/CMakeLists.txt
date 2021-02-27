add_library(CameraVLC STATIC "")

target_link_libraries(CameraVLC PUBLIC BaseFramework Camera 2DLayers libvlc xaudio2)

target_include_directories(CameraVLC PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(CameraVLC "")
else()
	GRAB_BASE_SOURCES(CameraVLC "")
endif()