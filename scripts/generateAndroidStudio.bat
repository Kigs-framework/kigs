@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET commandline= -DANDROID_NDK=%NDK% -DCMAKE_CXX_STANDARD=17 -DCMAKE_TOOLCHAIN_FILE=%NDK%/build/cmake/android.toolchain.cmake -DANDROID_ABI=x86_64 -DANDROID_PLATFORM=android-30 -DCMAKE_MAKE_PROGRAM=%NDK%/../../cmake/3.22.1/bin/ninja.exe

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionAndroidStudio MKDIR %myDir%\..\..\Build\solutionAndroidStudio

cd %myDir%\..\..\Build\solutionAndroidStudio

%cmakeCmd% %myDir% %commandline% -DKIGS_ANDROID=TRUE -G Ninja

%NDK%/../../cmake/3.22.1/bin/ninja.exe

cd %myDir%

PAUSE