cmake_minimum_required(VERSION 3.9.4)

project(all)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0076 NEW)

set(KIGS_ROOT "${CMAKE_SOURCE_DIR}/../../")
set(KIGS_FRAMEWORK "${KIGS_ROOT}/kigs/framework/")
set(KIGS_PRIVATE_FRAMEWORK "${KIGS_ROOT}/private/framework/")
set(KIGS_EXTERN_LIBS "${KIGS_ROOT}/kigs/libs/")
set(KIGS_PRIVATE_EXTERN_LIBS "${KIGS_ROOT}/kigs/libs/")


if("${CMAKE_GENERATOR}" MATCHES "(Win64|x64)" OR "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
	set(KIGS_PLATFORM_X64 TRUE)
else()
	set(KIGS_PLATFORM_X64 FALSE)
endif()

message(STATUS "is x64 ${KIGS_PLATFORM_X64}")

function(add_library)
	if((${ARGV1} STREQUAL "STATIC" OR ${ARGV1} STREQUAL "SHARED") AND "${ARGV2}" STREQUAL "")
		_add_library(${ARGV} "")
	else()
		_add_library(${ARGV})
	endif()

	if("${ARGV1}" STREQUAL "STATIC" AND NOT ("${ARGV2}" STREQUAL "IMPORTED"))
		string(TOUPPER ${ARGV0} NAMEUPPER)
		target_compile_definitions(${ARGV0} INTERFACE "KIGS_${NAMEUPPER}_AVAILABLE=1")
	endif()

	if(${KIGS_PLATFORM} STREQUAL "WUP" OR ${KIGS_PLATFORM} STREQUAL "Windows")
		target_compile_options(${ARGV0} INTERFACE "/wd4018" "/wd4244")
	endif()
	
	string(UUID project_uuid NAMESPACE "B12E62DA-F6A6-4CF5-B710-C2024B3D5547" NAME ${ARGV0} TYPE SHA1 UPPER)
	#message(STATUS "Setting ${ARGV0} to uid ${project_uuid}")
	set(${ARGV0}_GUID_CMAKE ${project_uuid} CACHE INTERNAL "${ARGV0} GUID")

	if("${ARGV1}" STREQUAL "SHARED" AND "${KIGS_PLATFORM}" STREQUAL "Android")
		get_property(xtra GLOBAL PROPERTY VS_GLOBAL_EXTRALINKDIRS)
		set_property(TARGET ${ARGV0} PROPERTY VS_GLOBAL_EXTRALINKDIRS ${xtra} "${CMAKE_BINARY_DIR}/../AndroidLibraries/$(TargetArchAbi)/$(Configuration)/" "${KIGS_ROOT}/Build/$(Platform)/$(Configuration)/")
	endif()
	
	if("${KIGS_PLATFORM}" STREQUAL "WUP" AND "${ARGV1}" STREQUAL "SHARED" )
		set_target_properties(${ARGV0} PROPERTIES VS_USER_PROPS "${CMAKE_BINARY_DIR}/WUP/WUP.props")
	endif()
endfunction()

function(add_executable)
	_add_executable(${ARGV} "")
endfunction()

function(set_kigsPlatform)
	set(KIGS_PLATFORM Windows PARENT_SCOPE)
	if(WIN32)
		if(${CMAKE_SYSTEM_NAME} MATCHES WindowsStore)
			set(KIGS_PLATFORM WUP PARENT_SCOPE)
		elseif(JAVASCRIPT)
			set(KIGS_PLATFORM Javascript PARENT_SCOPE)
		elseif(KIGS_ANDROID)
			set(KIGS_PLATFORM Android PARENT_SCOPE)
		elseif(KIGS_SWITCH)
			set(KIGS_PLATFORM Switch PARENT_SCOPE)
		else()
			set(KIGS_PLATFORM Windows PARENT_SCOPE)
		endif()

	elseif(APPLE) #TODO : ios / osX
		set(KIGS_PLATFORM iOS PARENT_SCOPE)
	
	elseif(JAVASCRIPT)
		set(KIGS_PLATFORM Javascript PARENT_SCOPE)

	elseif(UNIX) #TODO
		set(KIGS_PLATFORM Linux PARENT_SCOPE)
	endif()

endfunction(set_kigsPlatform)

set(CMAKE_CONFIGURATION_TYPES StaticDebug StaticRelease StaticReleaseTools)

if(WIN32)
	string (REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) 	# no WIN32
	string (REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}) # no _WINDOWS
endif()

set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS StaticDebug)
	
set_kigsPlatform()

# set(UNITY_BUILD 0)

add_library(Base INTERFACE)
add_library(Holographic INTERFACE)

target_sources(Holographic INTERFACE 
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/GazeManager.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/GazeManagerWithCollider.cpp"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/Holo3DSequenceManager.cpp")


set(CMAKE_C_FLAGS_STATICDEBUG "${CMAKE_C_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_STATICRELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_STATICRELEASETOOLS "${CMAKE_C_FLAGS_RELEASE}")

set(CMAKE_CXX_FLAGS_STATICDEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_STATICRELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_STATICRELEASETOOLS "${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_EXE_LINKER_FLAGS_STATICDEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")


if(WIN32 OR KIGS_ANDROID)
	if(USE_D3D)
		target_compile_definitions(Base INTERFACE USE_D3D)
	endif()
	if(${CMAKE_SYSTEM_NAME} MATCHES WindowsStore)
		target_compile_options(Base INTERFACE /std:c++17)
		target_compile_definitions(Base INTERFACE WUP _CRT_SECURE_NO_WARNINGS NOMINMAX)
		if(NOT USE_D3D)
			target_compile_definitions(Base INTERFACE GL_ES2)
		endif()

		if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM64" OR "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM")
			target_compile_definitions(Base INTERFACE KIGS_HOLOLENS2)
		endif()

		target_compile_options(Base INTERFACE /ZW /await)
		set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASE "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASE}")
		set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL")
		set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE}")
		set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL")
		set(KIGS_PLATFORM WUP)

		if(USE_D3D)
			set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WUPLibraries/D3D/${CMAKE_GENERATOR_PLATFORM})
		else()
			set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WUPLibraries/${CMAKE_GENERATOR_PLATFORM})
		endif()


		configure_file(${KIGS_ROOT}/kigs/scripts/WUP.props ${CMAKE_CURRENT_BINARY_DIR}/WUP/WUP.props COPYONLY)
	elseif(JAVASCRIPT)
		target_compile_definitions(Base INTERFACE JAVASCRIPT GL_ES2)
		set(KIGS_PLATFORM Javascript)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries)
		set (CMAKE_VS_PLATFORM_TOOLSET "Emscripten")
	elseif(KIGS_ANDROID)
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		target_compile_definitions(Base INTERFACE _M_ARM=7 GL_ES2 ANDROID)
		target_link_libraries(Base INTERFACE EGL GLESv2 log)
		set(KIGS_PLATFORM Android)
		
		if(NOT ANDROID_ABI)
	     	set(ANDROID_ABI armeabi-v7a)
			message(STATUS "set ANDROID_ABI to ${ANDROID_ABI}")
		endif()
		
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../AndroidLibraries/${ANDROID_ABI})
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../AndroidBinairies/${ANDROID_ABI})
	elseif(KIGS_SWITCH)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		target_compile_definitions(Base INTERFACE NN_PLATFORM_NX)
		set(KIGS_PLATFORM Switch)
		set (CMAKE_VS_PLATFORM_TOOLSET "NX")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../SwitchLibraries)
		configure_file( ${KIGS_ROOT}/private/scripts/NX.props ${CMAKE_CURRENT_BINARY_DIR}/NX/NX.props COPYONLY)
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
		target_compile_options(Base INTERFACE /std:c++17)
		target_compile_definitions(Base INTERFACE WIN32 _WINDOWS NOMINMAX _CRT_SECURE_NO_WARNINGS)
		set(CMAKE_EXE_LINKER_FLAGS_STATICDEBUG "${CMAKE_EXE_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
		set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASE "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASE} /SAFESEH:NO")
		set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS} /SAFESEH:NO /DEBUG:FULL")
		set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
		set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SAFESEH:NO")
		set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL /SAFESEH:NO")
		set(KIGS_PLATFORM Windows)
		if(KIGS_PLATFORM_X64)
			if(USE_D3D)
				set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WindowsLibraries/D3D/x64)
			else()
				set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WindowsLibraries/x64)
			endif()
		else()
			if(USE_D3D)
				set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WindowsLibraries/D3D)
			else()
				set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WindowsLibraries/)
			endif()
		endif()
		target_compile_options(Base INTERFACE /permissive-)
		target_link_libraries(Base INTERFACE Wininet Comctl32)
	endif()
	
	if(NOT KIGS_ANDROID)
		target_compile_options(Base INTERFACE $<$<CONFIG:StaticReleaseTools>:/Zi>)
		target_compile_options(Base INTERFACE $<$<CONFIG:StaticDebug>:/Zi>)
		target_compile_options(Base INTERFACE -bigobj)
	endif()
elseif(APPLE) #TODO : ios / osX
	set(KIGS_PLATFORM iOS)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../iOSLibraries)
elseif(JAVASCRIPT)
	target_compile_definitions(Base INTERFACE JAVASCRIPT GL_ES2)
	set(KIGS_PLATFORM Javascript)

	set(CMAKE_CXX_FLAGS_STATICRELEASE "${CMAKE_CXX_FLAGS_STATICRELEASE} --closure 1")

	if(${CMAKE_BUILD_TYPE} STREQUAL "StaticDebug")
	message(STATUS "building static debug config")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticDebug)
	elseif(${CMAKE_BUILD_TYPE} STREQUAL "StaticReleaseTools")
	message(STATUS "building static release tools")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticReleaseTools)
	elseif(${CMAKE_BUILD_TYPE} STREQUAL "StaticRelease")
	message(STATUS "building static release")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticRelease)
	endif()
	
elseif(UNIX) #TODO
	set(KIGS_PLATFORM Linux)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../LinuxLibraries)
endif()


if(EXISTS ${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
	set(KIGS_PLATFORM_ROOT ${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
else()
	set(KIGS_PLATFORM_ROOT ${KIGS_PRIVATE_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
endif()
message(STATUS "platform root : ${KIGS_PLATFORM_ROOT}")

target_compile_features(Base INTERFACE cxx_std_17)
target_compile_definitions(Base INTERFACE $<$<CONFIG:StaticDebug>:_DEBUG>)


add_library(BaseFramework INTERFACE)
target_link_libraries(BaseFramework INTERFACE Base)

target_compile_definitions(BaseFramework INTERFACE _KIGS_ONLY_STATIC_LIB_ LUA_COMPAT_5_2)
target_compile_definitions(BaseFramework INTERFACE $<$<CONFIG:StaticDebug>:KIGS_TOOLS>)
target_compile_definitions(BaseFramework INTERFACE $<$<CONFIG:StaticReleaseTools>:KIGS_TOOLS>)

target_include_directories(BaseFramework 
	INTERFACE 
		"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses"
		"${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Headers"
		"${KIGS_PRIVATE_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}"
		"${KIGS_PRIVATE_FRAMEWORK}/StandAloneUtilityClasses"
		"${KIGS_PRIVATE_FRAMEWORK}/StandAloneUtilityClasses/Headers"
		"${KIGS_EXTERN_LIBS}/cross_platform/ImGui"
		"${KIGS_EXTERN_LIBS}/cross_platform/lua/src")

macro(GRAB_UNITY_BUILD_SOURCES TARGET_NAME SPECIFIC_DIR)
	file(GLOB unity_build_sources "Sources/*.cpp" "${SPECIFIC_DIR}/Sources/*.cpp" "Sources/*.c" "${SPECIFIC_DIR}/Sources/*.c")
	set(ALLCPP "")
	foreach(item ${unity_build_sources})
		STRING(REGEX MATCH ".*all\\.(cpp|c)$" allfiles ${item})
		if(allfiles)
			LIST(APPEND ALLCPP ${allfiles})
		endif()
	endforeach()
	file(GLOB unity_build_headers "Headers/*.hpp" "${SPECIFIC_DIR}/Headers/*.hpp" "Headers/*.h" "${SPECIFIC_DIR}/Headers/*.h")
	LIST(APPEND ALLCPP ${unity_build_headers})
	target_sources(${TARGET_NAME} 
		PRIVATE 
			${ALLCPP})
endmacro()

macro(add_kigs_module MODULE_NAME)
	if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE_NAME})
		add_subdirectory(${MODULE_NAME})
		SET_TARGET_PROPERTIES(${MODULE_NAME} PROPERTIES  FOLDER "framework" )
	endif()
endmacro()

function(GRAB_BASE_SOURCES TARGET_NAME EXCLUDED_FILES)

	file(GLOB_RECURSE BASE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Sources/*.cpp" "Sources/*.c")
	set(ALLCPP "")
	set(TOREMOVE "")

	foreach(item ${BASE_SOURCES})
		STRING(REGEX MATCH ".*all\\.(cpp|c)$" allfiles ${item})
		if(allfiles)
			LIST(APPEND ALLCPP ${allfiles})
		endif()
		
		STRING(REGEX MATCH ".*PrecompiledHeaders\\.(cpp|c)$" precomfiles ${item})
		if(precomfiles)
			LIST(APPEND TOREMOVE ${precomfiles})
		endif()

		foreach(reg ${EXCLUDED_FILES})
			STRING(REGEX MATCH ${reg} excluded ${item})
			if(excluded)
				LIST(APPEND TOREMOVE ${excluded})
			endif()
		endforeach()

	endforeach()

	list(LENGTH ALLCPP COUNT)
	if(${COUNT})
		list(REMOVE_ITEM BASE_SOURCES ${ALLCPP})
	endif()

	file(GLOB_RECURSE BASE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "Headers/*.h" "Sources/*.hpp")
	LIST(APPEND BASE_SOURCES ${BASE_HEADERS})

	list(LENGTH TOREMOVE COUNT)
	if(${COUNT})
		list(REMOVE_ITEM BASE_SOURCES ${TOREMOVE})
	endif()

	target_sources(${TARGET_NAME} PRIVATE ${BASE_SOURCES})
endfunction()
	


function(GRAB_PLATFORM_SOURCES TARGET_NAME SPECIFIC_DIR EXCLUDED_FILES)

	file(GLOB_RECURSE BASE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${SPECIFIC_DIR}/Sources/*.cpp" "${SPECIFIC_DIR}/Sources/*.c")
	set(ALLCPP "")
	set(TOREMOVE "")

	foreach(item ${BASE_SOURCES})
		STRING(REGEX MATCH ".*all\\.(cpp|c)$" allfiles ${item})
		if(allfiles)
			LIST(APPEND ALLCPP ${allfiles})
		endif()
		
		STRING(REGEX MATCH ".*PrecompiledHeaders\\.(cpp|c)$" precomfiles ${item})
		if(precomfiles)
			LIST(APPEND TOREMOVE ${precomfiles})
		endif()

		foreach(reg ${EXCLUDED_FILES})
			STRING(REGEX MATCH ${reg} excluded ${item})
			if(excluded)
				LIST(APPEND TOREMOVE ${excluded})
			endif()
		endforeach()

	endforeach()
	
	list(LENGTH ALLCPP COUNT)
	if(${COUNT})
		list(REMOVE_ITEM BASE_SOURCES ${ALLCPP})
	endif()

	list(LENGTH TOREMOVE COUNT)
	if(${COUNT})
		list(REMOVE_ITEM BASE_SOURCES ${TOREMOVE})
	endif()

	target_sources(${TARGET_NAME} PRIVATE ${BASE_SOURCES})
endfunction()


add_library(BaseExecutable INTERFACE)
if(${KIGS_PLATFORM} STREQUAL "Windows")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainConsole.cpp")
elseif(${KIGS_PLATFORM} STREQUAL "WUP")
	if(USE_D3D)
		target_sources(BaseExecutable
			INTERFACE 
				"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/BaseAppDX11.cpp")
	else()
		target_sources(BaseExecutable
			INTERFACE 
				"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/BaseApp.cpp")
	endif()
	#target_link_libraries(BaseExecutable INTERFACE angle_dll_package)
elseif(${KIGS_PLATFORM} STREQUAL "Android")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_PRIVATE_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/main.cpp")
elseif(${KIGS_PLATFORM} STREQUAL "Javascript")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainJavascript.cpp")
endif()

if(${KIGS_PLATFORM} STREQUAL "Windows")
	add_library(BaseDLL INTERFACE)
	target_sources(BaseDLL 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainDll.cpp")
			
	add_library(QtKigsLib INTERFACE)
	target_sources(QtKigsLib INTERFACE "${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainLib.cpp")
	target_link_libraries(QtKigsLib INTERFACE 2DLayers Renderer ImGui LuaKigsBind Core CoreAnimation)
			
	add_library(DataDrivenDllApplication INTERFACE)
	target_sources(DataDrivenDllApplication INTERFACE "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/DataDrivenBaseApplication.cpp")
	target_link_libraries(DataDrivenDllApplication INTERFACE BaseDLL 2DLayers Renderer ImGui LuaKigsBind CoreDataDrivenSequence CoreAnimation)
	
endif()

add_library(BaseApplication INTERFACE)
target_link_libraries(BaseApplication INTERFACE BaseExecutable Core FileManager XML)

add_library(DataDrivenApplication INTERFACE)
target_sources(DataDrivenApplication INTERFACE "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/DataDrivenBaseApplication.cpp")
target_link_libraries(DataDrivenApplication INTERFACE BaseExecutable 2DLayers Renderer LuaKigsBind CoreDataDrivenSequence CoreAnimation ImGui)

IF(EXISTS ${KIGS_ROOT}/kigs)
	message(STATUS "CMake enter kigs")
	add_subdirectory(${KIGS_ROOT}/kigs kigs)
endif()

IF(EXISTS ${KIGS_ROOT}/private)
	message(STATUS "CMake enter private")
	add_subdirectory(${KIGS_ROOT}/private private)
endif()
