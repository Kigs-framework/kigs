project(KigsPackager)

add_executable(KigsPackager "")

set(KigsPackager_GUID_CMAKE "1C3AEB3D-6195-4F7F-8159-F74AE83702DF" CACHE INTERNAL "KigsPackager GUID")

target_link_libraries(KigsPackager PRIVATE BaseApplication)

target_include_directories(KigsPackager PRIVATE "Headers")


file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(KigsPackager 
	PRIVATE 
		${all_sources}
		${all_headers}
		)



