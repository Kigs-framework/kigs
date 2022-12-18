@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 17 2022

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionAndroidVSCMake MKDIR %myDir%\..\..\Build\solutionAndroidVSCMake

cd %myDir%\..\..\Build\solutionAndroidVSCMake

%cmakeCmd% %myDir% -A Win32 -G"%targetName%" -DKIGS_ANDROID=TRUE 

REM call VcxprojWinToAndroid.exe utility on generated .vcxproj and .sln
for /f "delims=" %%a in ('dir /b /s *.vcxproj') do (
    "./../../kigs/tools/win32_executable/VcxprojWinToAndroid.exe" -android -api=android-23 "%%a"
)

for /f "delims=" %%a in ('dir /b /s *.sln') do (
    "./../../kigs/tools/win32_executable/VcxprojWinToAndroid.exe" -android -api=android-23 "%%a"
)

PAUSE