add_library(OMeshSimplification INTERFACE)

target_include_directories(OMeshSimplification INTERFACE "Headers")

file(GLOB_RECURSE allcpps "Sources/*")
target_sources(	OMeshSimplification INTERFACE ${allcpps}
				"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/OctreeBase.cpp")
