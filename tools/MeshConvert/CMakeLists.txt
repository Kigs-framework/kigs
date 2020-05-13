project(MeshConvert)

add_executable(MeshConvert "")

set(MeshConvert_GUID_CMAKE "1C3AEB3D-6195-4F7F-8159-F74AE83702DF" CACHE INTERNAL "MeshConvert GUID")

target_link_libraries(MeshConvert PRIVATE DataDrivenApplication Collision GenericAnimation)

if(${FBXSDK_AVAILABLE})
target_link_libraries(MeshConvert PRIVATE fbxsdk)
target_compile_definitions(MeshConvert PRIVATE USE_FBXSDK)
endif()

target_include_directories(MeshConvert PRIVATE "Headers")

file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")

target_sources(MeshConvert 
	PRIVATE 
		${all_sources}
		${all_headers}
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/Base3DImporter.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/ColladaParser.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/FBXImport.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/OBJImport.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/PLYImport.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/STLImport.cpp"
		)



