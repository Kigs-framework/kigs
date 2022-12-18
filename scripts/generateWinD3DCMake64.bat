@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 17 2022

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWinDX11CMake64 MKDIR %myDir%\..\..\Build\solutionWinDX11CMake64

cd %myDir%\..\..\Build\solutionWinDX11CMake64
REM  -DUNITY_BUILD=TRUE
%cmakeCmd% %myDir% -DUNITY_BUILD=TRUE -DUSE_D3D=TRUE -G"%targetName%" -A x64
pause
