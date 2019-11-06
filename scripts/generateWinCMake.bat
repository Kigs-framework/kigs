@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 16 2019

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWinCMake MKDIR %myDir%\..\..\Build\solutionWinCMake

cd %myDir%\..\..\Build\solutionWinCMake
REM  -DUNITY_BUILD=TRUE
%cmakeCmd% %myDir% -DUNITY_BUILD=TRUE -G"%targetName%" -A Win32
pause
