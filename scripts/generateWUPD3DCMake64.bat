@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 17 2022

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWUPD3DCMake64 MKDIR %myDir%\..\..\Build\solutionWUPD3DCMake64

cd %myDir%\..\..\Build\solutionWUPD3DCMake64

%cmakeCmd% %myDir% -G"%targetName%" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0.18362.0 -DUNITY_BUILD=FALSE -DUSE_D3D=TRUE

PAUSE