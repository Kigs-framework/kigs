add_library(LuaKigsBind STATIC "")

target_link_libraries(LuaKigsBind PUBLIC CoreAnimation GUI 2DLayers Input lua)

target_include_directories(LuaKigsBind PUBLIC "Headers")

target_include_directories(LuaKigsBind PUBLIC "../../ExternalLibraries/ExternWin32Libraries/lua/src")


if(UNITY_BUILD)
	GRAB_UNITY_BUILD_SOURCES(LuaKigsBind "")
else()
	GRAB_BASE_SOURCES(LuaKigsBind "")
endif()
