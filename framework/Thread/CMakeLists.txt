add_library(Thread STATIC "")

target_link_libraries(Thread PUBLIC BaseFramework Core)

target_include_directories(Thread PUBLIC "Headers")

set(PLATFORM_MODULE "Thread${KIGS_PLATFORM}")

target_include_directories(Thread PRIVATE "${PLATFORM_MODULE}/Headers")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(Thread ${PLATFORM_MODULE})
else()
	GRAB_BASE_SOURCES(Thread "")
	GRAB_PLATFORM_SOURCES(Thread ${PLATFORM_MODULE} "")
endif()
