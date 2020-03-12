add_library(Thread STATIC "")

target_link_libraries(Thread PUBLIC BaseFramework Core)

target_include_directories(Thread PUBLIC "Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Thread "")
else()
	GRAB_BASE_SOURCES(Thread "")
endif()
