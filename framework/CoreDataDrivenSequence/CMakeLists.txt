add_library(CoreDataDrivenSequence STATIC "")

target_link_libraries(CoreDataDrivenSequence PUBLIC Core)

target_include_directories(CoreDataDrivenSequence PUBLIC "Headers")

if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(CoreDataDrivenSequence "")
else()
	GRAB_BASE_SOURCES(CoreDataDrivenSequence "")
endif()
