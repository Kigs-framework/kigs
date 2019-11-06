@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 16 2019

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWinDX11CMake MKDIR %myDir%\..\..\Build\solutionWinDX11CMake

cd %myDir%\..\..\Build\solutionWinDX11CMake
REM  -DUNITY_BUILD=TRUE
%cmakeCmd% %myDir% -DUSE_D3D=TRUE -G"%targetName%" -A Win32
pause
