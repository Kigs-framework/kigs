
add_library(HTTPRequest STATIC "")

target_link_libraries(HTTPRequest PUBLIC Core XML)
target_include_directories(HTTPRequest PUBLIC "Headers")

set(PLATFORM_MODULE "HTTPRequest${KIGS_PLATFORM}")

target_include_directories(HTTPRequest 
	PRIVATE 
		"${PLATFORM_MODULE}/Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(HTTPRequest ${PLATFORM_MODULE})
else()
	GRAB_BASE_SOURCES(HTTPRequest "")
	GRAB_PLATFORM_SOURCES(HTTPRequest ${PLATFORM_MODULE} "")
endif()
