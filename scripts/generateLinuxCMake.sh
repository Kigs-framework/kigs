#! /bin/bash

export myDir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

export targetName="Unix Makefiles"


echo selected target : $targetName
echo current dir     : $myDir

if [ ! -d "$myDir/../../Build" ]; then
  mkdir  "$myDir/../../Build"
fi

if [ ! -d "$myDir/../../Build/solutionLinuxCMake" ]; then
  mkdir  "$myDir/../../Build/solutionLinuxCMake"
fi

if [ ! -d "$myDir/../../Build/solutionLinuxCMake/StaticDebug" ]; then
  mkdir  "$myDir/../../Build/solutionLinuxCMake/StaticDebug"
fi

if [ ! -d "$myDir/../../Build/solutionLinuxCMake/StaticRelease" ]; then
  mkdir  "$myDir/../../Build/solutionLinuxCMake/StaticRelease"
fi

if [ ! -d "$myDir/../../Build/solutionLinuxCMake/StaticReleaseTools" ]; then
  mkdir  "$myDir/../../Build/solutionLinuxCMake/StaticReleaseTools"
fi

cd "$myDir/../../Build/solutionLinuxCMake/StaticDebug"

echo launching cmake debug
cmake ../../../kigs/scripts/ -DLINUX=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticDebug

cd "$myDir/../../Build/solutionLinuxCMake/StaticRelease"

echo launching cmake release
cmake ../../../kigs/scripts/ -DLINUX=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticRelease

cd "$myDir/../../Build/solutionLinuxCMake/StaticReleaseTools"

echo launching cmake releasetools
cmake ../../../kigs/scripts/ -DLINUX=TRUE -G"$targetName" -DCMAKE_BUILD_TYPE=StaticReleaseTools