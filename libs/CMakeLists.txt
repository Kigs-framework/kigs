project(libs)

# imgui
message(STATUS "add imgui")
add_subdirectory("cross_platform/ImGui")

# jpeg
message(STATUS "add jpeg")
add_subdirectory("cross_platform/jpeg")
SET_TARGET_PROPERTIES( jpeg   					PROPERTIES  FOLDER "libs" )

# png
message(STATUS "add png")
add_subdirectory("cross_platform/png")
SET_TARGET_PROPERTIES( png   					PROPERTIES  FOLDER "libs" )

# zlib
message(STATUS "add zlib")
add_subdirectory("cross_platform/zlib")
SET_TARGET_PROPERTIES( zlib   					PROPERTIES  FOLDER "libs" )

# lua
message(STATUS "add lua")
add_subdirectory("cross_platform/lua")
SET_TARGET_PROPERTIES( lua   					PROPERTIES  FOLDER "libs" )