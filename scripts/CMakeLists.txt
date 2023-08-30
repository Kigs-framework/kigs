cmake_minimum_required(VERSION 3.9.4)

project(all)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

cmake_policy(SET CMP0076 NEW)

if(NOT DEFINED KIGS_ROOT)
	set(KIGS_ROOT "${CMAKE_SOURCE_DIR}/../../")
endif()	

set(KIGS_FRAMEWORK "${KIGS_ROOT}/kigs/framework/")
set(KIGS_PRIVATE_FRAMEWORK "${KIGS_ROOT}/private/framework/")
set(KIGS_EXTERN_LIBS "${KIGS_ROOT}/kigs/libs/")
set(KIGS_PRIVATE_EXTERN_LIBS "${KIGS_ROOT}/kigs/libs/")
set(KIGS_SIDEPROJECTS "${KIGS_ROOT}/sideProjects/")
set(KIGS_PUBLICPROJECTS "${KIGS_ROOT}/publicKigsProjects/")
set(KIGS_TESTS "${KIGS_ROOT}/KigsTests/")

# 3 possible host

string(FIND "${CMAKE_HOST_SYSTEM_NAME}" "Windows" IS_Windows )


if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
   set(HOST_PLATFORM "WINDOWS")
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    set(HOST_PLATFORM "APPLE")
else()
     # on Linux check WSL
	 execute_process(COMMAND bash "-c" "echo $(grep Microsoft /proc/version)" OUTPUT_VARIABLE CHECK_LINUX_VERSION)
	 string(FIND "${CHECK_LINUX_VERSION}" "Microsoft" IS_WSL )

	 if(${IS_WSL} EQUAL -1) # IS_WSL -1 = not found
		set(HOST_PLATFORM "LINUX")
	 else()
	    set (HOST_PLATFORM "WINDOWS") # WSL can execute .exe programs so considered WINDOWS
	 endif()
endif()

function(add_library)
	if(("${ARGV1}" STREQUAL "STATIC" OR "${ARGV1}" STREQUAL "SHARED") AND "${ARGV2}" STREQUAL "")
		_add_library(${ARGV} "")
	else()
		_add_library(${ARGV})
	endif()

    # add defines for each librairies
	if("${ARGV1}" STREQUAL "STATIC" AND NOT ("${ARGV2}" STREQUAL "IMPORTED"))
		string(TOUPPER ${ARGV0} NAMEUPPER)
		target_compile_definitions(${ARGV0} INTERFACE "KIGS_${NAMEUPPER}_AVAILABLE=1")
	endif()

	if("${KIGS_PLATFORM}" STREQUAL "WUP" OR "${KIGS_PLATFORM}" STREQUAL "Windows")
		target_compile_options(${ARGV0} INTERFACE "/wd4018" "/wd4244")
	endif()
	
	string(UUID project_uuid NAMESPACE "B12E62DA-F6A6-4CF5-B710-C2024B3D5547" NAME ${ARGV0} TYPE SHA1 UPPER)
	set(${ARGV0}_GUID_CMAKE ${project_uuid} CACHE INTERNAL "${ARGV0} GUID")

    # add some properties for WUP platform
	if("${KIGS_PLATFORM}" STREQUAL "WUP" AND "${ARGV1}" STREQUAL "SHARED" )
		set_target_properties(${ARGV0} PROPERTIES VS_USER_PROPS "${CMAKE_BINARY_DIR}/WUP/WUP.props")
	endif()

	if("${KIGS_PLATFORM}" STREQUAL "WUP" AND NOT "${ARGV1}" STREQUAL "INTERFACE")
		set_target_properties(${ARGV0} PROPERTIES VS_WINDOWS_TARGET_PLATFORM_MIN_VERSION "10.0.18362.0")
	endif()

endfunction()

function(add_executable)
	_add_executable(${ARGV} "")
endfunction()


# set kigs platform methods
function(set_kigsPlatform)

	message(STATUS "set kigs platform")
	message(STATUS ${HOST_PLATFORM})

    # default is Windows
	set(KIGS_PLATFORM Windows CACHE INTERNAL "")
	
	# Windows host build can be UWP, Android with Android Studio and Windows with Visual Studio
	if("${HOST_PLATFORM}" STREQUAL "WINDOWS")
		message(STATUS "Host Windows")
		if("${CMAKE_SYSTEM_NAME}" MATCHES WindowsStore)
			set(KIGS_PLATFORM WUP CACHE INTERNAL "")
		elseif(JAVASCRIPT) # emscripten in WSL
			set(KIGS_PLATFORM Javascript CACHE INTERNAL "")
		elseif(KIGS_ANDROID)
			set(KIGS_PLATFORM Android CACHE INTERNAL "")
		else()
			set(KIGS_PLATFORM Windows CACHE INTERNAL "")
		endif()
	elseif("${HOST_PLATFORM}" STREQUAL "APPLE") #TODO iOS platform obsolete
		message(STATUS "Host Apple")
		set(KIGS_PLATFORM iOS CACHE INTERNAL "")
	elseif("${HOST_PLATFORM}" STREQUAL "LINUX")  # on Linux 
		message(STATUS "Host Linux")
		if(JAVASCRIPT)
			set(KIGS_PLATFORM Javascript CACHE INTERNAL "")
		elseif(LINUX)
			set(KIGS_PLATFORM Linux CACHE INTERNAL "")
		elseif(KIGS_ANDROID) 
			set(KIGS_PLATFORM Android CACHE INTERNAL "")
		endif()
	endif()
	
	message(STATUS "building for platform " "${KIGS_PLATFORM}" )
	
endfunction(set_kigsPlatform)

set(CMAKE_CONFIGURATION_TYPES StaticDebug StaticRelease StaticReleaseTools)

set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS StaticDebug)
	
set_kigsPlatform()

string (REPLACE "/DWIN32" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") 	# no WIN32
string (REPLACE "/D_WINDOWS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # no _WINDOWS


add_library(Base INTERFACE)

add_library(BaseFramework INTERFACE)
if("${HOST_PLATFORM}" STREQUAL "WINDOWS")
	add_library(Holographic INTERFACE)
	if(USE_D3D) #D3D only on windows host
		target_compile_definitions(Base INTERFACE USE_D3D)
	endif()
	target_sources(Holographic INTERFACE "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/Holo3DSequenceManager.cpp")

endif()


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

set(CMAKE_MODULE_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_MODULE_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_MODULE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")


if("${KIGS_PLATFORM}" STREQUAL "WUP") 
	target_compile_options(Base INTERFACE /utf-8)
	target_compile_definitions(Base INTERFACE WUP _CRT_SECURE_NO_WARNINGS NOMINMAX)
	if(NOT USE_D3D)
		target_compile_definitions(Base INTERFACE GL_ES2)
	endif()

	if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM64" OR "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM")
		target_compile_definitions(Base INTERFACE KIGS_HOLOLENS2)
	endif()

	target_compile_options(Base INTERFACE /await)
	set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASE "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASE}")
	set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE}")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL")

	if(USE_D3D)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WUPLibraries/D3D/${CMAKE_GENERATOR_PLATFORM})
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../WUPLibraries/${CMAKE_GENERATOR_PLATFORM})
	endif()

	configure_file(${KIGS_ROOT}/kigs/scripts/WUP.props ${CMAKE_CURRENT_BINARY_DIR}/WUP/WUP.props COPYONLY)
	
elseif("${KIGS_PLATFORM}" STREQUAL "Javascript") 
	target_compile_definitions(Base INTERFACE JAVASCRIPT GL_ES2)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries)
	if("${CMAKE_BUILD_TYPE}" STREQUAL "StaticDebug")
	message(STATUS "building static debug config")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticDebug)
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "StaticReleaseTools")
	message(STATUS "building static release tools")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticReleaseTools)
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "StaticRelease")
	message(STATUS "building static release")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../EmscriptenLibraries/StaticRelease)
	endif()		
elseif("${KIGS_PLATFORM}" STREQUAL "Linux") 
	target_compile_definitions(Base INTERFACE LINUX)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../LinuxLibraries)		
	
	if("${CMAKE_BUILD_TYPE}" STREQUAL "StaticDebug")
		message(STATUS "building static debug config")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../LinuxLibraries/StaticDebug)
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "StaticReleaseTools")
		message(STATUS "building static release tools")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../LinuxLibraries/StaticReleaseTools)
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "StaticRelease")
		message(STATUS "building static release")
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../LinuxLibraries/StaticRelease)
	endif()
	
elseif("${KIGS_PLATFORM}" STREQUAL "iOS")  #TODO
	target_compile_definitions(Base INTERFACE IOS)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../iOSLibraries)
elseif("${KIGS_PLATFORM}" STREQUAL "Android") 
	target_compile_options(Base INTERFACE -frtti)
	target_compile_definitions(Base INTERFACE _M_ARM=7 GL_ES2 ANDROID)
	
	# Searches for a package provided by the game activity dependency
	find_package(game-activity REQUIRED CONFIG)
	
	target_link_libraries(Base INTERFACE game-activity::game-activity EGL GLESv3 jnigraphics android log)
	
	if(NOT ANDROID_ABI)
		set(ANDROID_ABI armeabi-v7a)
		message(STATUS "set ANDROID_ABI to ${ANDROID_ABI}")
	endif()
	if(NOT TargetArchAbi)
		set(TargetArchAbi armeabi-v7a)
		message(STATUS "set TargetArchAbi to ${TargetArchAbi}")
	endif()
	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../AndroidLibraries/${ANDROID_ABI})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../AndroidBinairies/${ANDROID_ABI})

    # Android Studio don't use StaticDebug, StaticRelease & StaticReleaseTools build config 
	target_compile_definitions(BaseFramework INTERFACE $<$<CONFIG:Debug>:KIGS_TOOLS>)

else() #windows 32 or 64
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	target_compile_options(Base INTERFACE /utf-8)
	target_compile_definitions(Base INTERFACE WIN32 _WINDOWS NOMINMAX _CRT_SECURE_NO_WARNINGS)
	set(CMAKE_EXE_LINKER_FLAGS_STATICDEBUG "${CMAKE_EXE_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASE "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASE} /SAFESEH:NO")
	set(CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_EXE_LINKER_FLAGS_STATICRELEASETOOLS} /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG "${CMAKE_SHARED_LINKER_FLAGS_STATICDEBUG} /SAFESEH:NO /DEBUG:FULL")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASE} /SAFESEH:NO")
	set(CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS "${CMAKE_SHARED_LINKER_FLAGS_STATICRELEASETOOLS} /DEBUG:FULL /SAFESEH:NO")

	if("${CMAKE_GENERATOR}" MATCHES "(Win64|x64)" OR "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
		set(KIGS_PLATFORM_X64 TRUE)
	else()
		set(KIGS_PLATFORM_X64 FALSE)
	endif()

	message(STATUS "is x64 ${KIGS_PLATFORM_X64}")
	
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

if(MSVC)
	target_compile_options(Base INTERFACE $<$<CONFIG:StaticReleaseTools>:/Zi>)
	if("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM64" OR "${CMAKE_GENERATOR_PLATFORM}" STREQUAL "ARM")
		target_compile_options(Base INTERFACE $<$<CONFIG:StaticDebug>:/Zi>)
	else()
		target_compile_options(Base INTERFACE $<$<CONFIG:StaticDebug>:/ZI>)
	endif()
	target_compile_options(Base INTERFACE /bigobj)
	target_link_options(Base INTERFACE $<$<CONFIG:StaticReleaseTools>:/INCREMENTAL>)
	target_link_options(Base INTERFACE $<$<CONFIG:StaticDebug>:/INCREMENTAL>)
	target_compile_options(Base INTERFACE $<$<CONFIG:StaticRelease>:/Zi>)
	target_link_options(Base INTERFACE $<$<CONFIG:StaticRelease>:/DEBUG:FULL>)
endif()


if(EXISTS ${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
	set(KIGS_PLATFORM_ROOT ${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
else()
	set(KIGS_PLATFORM_ROOT ${KIGS_PRIVATE_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM})
endif()
message(STATUS "platform root : ${KIGS_PLATFORM_ROOT}")

target_compile_features(Base INTERFACE cxx_std_23)
target_compile_definitions(Base INTERFACE $<$<CONFIG:StaticDebug>:_DEBUG>)


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
if("${KIGS_PLATFORM}" STREQUAL "Windows")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainConsole.cpp")
elseif("${KIGS_PLATFORM}" STREQUAL "WUP")
	if(USE_D3D)
		target_sources(BaseExecutable
			INTERFACE 
				"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/BaseAppDX11.cpp")
	else()
		target_sources(BaseExecutable
			INTERFACE 
				"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/BaseApp.cpp")
	endif()
	target_link_libraries(BaseExecutable INTERFACE vccorlib)
elseif("${KIGS_PLATFORM}" STREQUAL "Android")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/main.cpp")
elseif("${KIGS_PLATFORM}" STREQUAL "Javascript")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainJavascript.cpp")
elseif("${KIGS_PLATFORM}" STREQUAL "Linux")
	target_sources(BaseExecutable 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainLinux.cpp")			
endif()

if("${KIGS_PLATFORM}" STREQUAL "Windows")
	add_library(BaseDLL INTERFACE)
	target_sources(BaseDLL 
		INTERFACE 
			"${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainDll.cpp")
			
	add_library(QtKigsLib INTERFACE)
	target_sources(QtKigsLib INTERFACE "${KIGS_FRAMEWORK}/PlatformsModules/${KIGS_PLATFORM}/Platform/Main/mainLib.cpp")
	target_link_libraries(QtKigsLib INTERFACE 2DLayers Renderer ImGui LuaKigsBind Core CoreAnimation)
			
	add_library(DataDrivenDllApplication INTERFACE)
	target_sources(DataDrivenDllApplication INTERFACE "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/DataDrivenBaseApplication.cpp")
	target_link_libraries(DataDrivenDllApplication INTERFACE BaseDLL 2DLayers Renderer ImGui LuaKigsBind CoreAnimation)
	
endif()

add_library(BaseApplication INTERFACE)
target_link_libraries(BaseApplication INTERFACE BaseExecutable Core FileManager XML)

add_library(DataDrivenApplication INTERFACE)
target_sources(DataDrivenApplication INTERFACE "${KIGS_FRAMEWORK}/StandAloneUtilityClasses/Sources/DataDrivenBaseApplication.cpp")
target_link_libraries(DataDrivenApplication INTERFACE BaseExecutable 2DLayers Renderer LuaKigsBind CoreAnimation ImGui)

IF(EXISTS ${KIGS_ROOT}/kigs)
	message(STATUS "CMake enter kigs")
	add_subdirectory(${KIGS_ROOT}/kigs kigs)
endif()

IF(EXISTS ${KIGS_ROOT}/private)
	message(STATUS "CMake enter private")
	add_subdirectory(${KIGS_ROOT}/private private)
endif()

if(NOT "${KIGS_PLATFORM}" STREQUAL "Android")

	IF(EXISTS ${KIGS_SIDEPROJECTS})
		message(STATUS "CMake enter sideProjects/")
		add_subdirectory(${KIGS_SIDEPROJECTS} sideProjects)
	endif()

	IF(EXISTS ${KIGS_PUBLICPROJECTS})
		message(STATUS "CMake enter public Kigs projects/")
		add_subdirectory(${KIGS_PUBLICPROJECTS} publicKigsProjects)
	endif()

	IF(EXISTS ${KIGS_TESTS})
		message(STATUS "CMake enter Kigs tests projects/")
		add_subdirectory(${KIGS_TESTS} KigsTests)
	endif()

endif()