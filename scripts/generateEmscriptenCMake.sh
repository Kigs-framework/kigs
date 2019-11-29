#! /bin/bash

export myDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export targetName="Unix Makefiles"


echo selected target : $targetName
echo current dir     : $myDir

if [ ! -d "$myDir/../../Build" ]; then
  mkdir  "$myDir/../../Build"
fi

if [ ! -d "$myDir/../../Build/solutionEmscriptenCMake" ]; then
  mkdir  "$myDir/../../Build/solutionEmscriptenCMake"
fi

if [ ! -d "$myDir/../../Build/solutionEmscriptenCMake/StaticDebug" ]; then
  mkdir  "$myDir/../../Build/solutionEmscriptenCMake/StaticDebug"
fi

if [ ! -d "$myDir/../../Build/solutionEmscriptenCMake/StaticRelease" ]; then
  mkdir  "$myDir/../../Build/solutionEmscriptenCMake/StaticRelease"
fi

if [ ! -d "$myDir/../../Build/solutionEmscriptenCMake/StaticReleaseTools" ]; then
  mkdir  "$myDir/../../Build/solutionEmscriptenCMake/StaticReleaseTools"
fi

cd "$myDir/../../Build/solutionEmscriptenCMake/StaticDebug"

echo launching cmake debug
emconfigure cmake ../../../kigs/Scripts/ -DJAVASCRIPT=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticDebug

cd "$myDir/../../Build/solutionEmscriptenCMake/StaticRelease"

echo launching cmake release
emconfigure cmake ../../../kigs/Scripts/ -DJAVASCRIPT=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticRelease

cd "$myDir/../../Build/solutionEmscriptenCMake/StaticReleaseTools"

echo launching cmake releasetools
emconfigure cmake ../../../kigs/Scripts/ -DJAVASCRIPT=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticReleaseTools