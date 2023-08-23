# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("sample8android")

set(KIGS_ANDROID TRUE)

set(KIGS_ROOT "${CMAKE_SOURCE_DIR}/../../../../../../../../")
add_subdirectory(${KIGS_ROOT}/kigs/scripts/ ${KIGS_ROOT}/Build/AndroidCMake/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}/)

add_subdirectory(${KIGS_ROOT}/kigs/projects/Sample8 ${KIGS_ROOT}/Build/AndroidCMake/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}/Sample8)

#copy assets files in android assets directory (done in prebuild)

#if(EXISTS ${KIGS_ROOT}/kigs/projects/Sample8/assets)
#	file(COPY ${KIGS_ROOT}/kigs/projects/Sample8/assets DESTINATION ${CMAKE_SOURCE_DIR}/../assets)
#endif()


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
		empty.cpp
		)

target_link_libraries(${CMAKE_PROJECT_NAME}
      # List libraries link to the target library
       Sample8
        )
