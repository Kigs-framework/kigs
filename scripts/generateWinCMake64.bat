@ECHO off

SET myDir=%~dp0

SET cmakeCmd=cmake
SET targetName=Visual Studio 17 2022

ECHO selected target : %targetName%
ECHO current dir     : %myDir%

IF NOT EXIST %myDir%\..\..\Build\                 MKDIR %myDir%\..\..\Build\
IF NOT EXIST %myDir%\..\..\Build\solutionWinCMakeWin64 MKDIR %myDir%\..\..\Build\solutionWinCMakeWin64

cd %myDir%\..\..\Build\solutionWinCMakeWin64
REM  -DUNITY_BUILD=TRUE
%cmakeCmd% %myDir% -DUNITY_BUILD=FALSE -G"%targetName%" -A x64
pause
