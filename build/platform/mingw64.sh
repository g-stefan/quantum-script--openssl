#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

export XYO_PLATFORM=mingw64
export XYO_PATH_REPOSITORY=$HOME/SDK/mingw64
export XYO_PATH_RELEASE=$HOME/SDK/mingw64/release
export PATH=$PATH:/c/msys64/mingw64/bin/../libexec;

. ./build/ubuntu.config.sh

export WSL_BUILD_PROCESS_PATH=$HOME/SDK/mingw64/source/$BUILD_PROJECT

/bin/sh -- ./build/platform/wsl.process.sh $1
RETV=$?

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
