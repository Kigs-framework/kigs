add_library(fsm INTERFACE)

add_subdirectory(GPSCourse)

add_subdirectory(XMLArchive)

add_subdirectory(CoreFSM)

add_subdirectory(OctreeMeshSimplification)

target_sources(fsm INTERFACE 
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/FSM/FSM.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/FSM/FSM_State.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/Sources/FSM/FSM_StateStruct.cpp")
