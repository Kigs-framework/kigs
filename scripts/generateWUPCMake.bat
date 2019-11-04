@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 16 2019

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWUPCMake MKDIR %myDir%\..\..\Build\solutionWUPCMake

cd %myDir%\..\..\Build\solutionWUPCMake

%cmakeCmd% %myDir% -G"%targetName%" -A Win32 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0.17763.0 -DUNITY_BUILD=TRUE

PAUSE