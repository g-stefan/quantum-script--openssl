#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

export XYO_PATH_REPOSITORY=$HOME/SDK
export XYO_PATH_RELEASE=$HOME/SDK/release

. ./build/ubuntu.config.sh

# On WSL we must build on home folder to keep executable bits on files
WSL_BUILD_PROCESS_PATH=$HOME/SDK/source/$BUILD_PROJECT

. ./build/platform/wsl.process.sh $1
RETV=$?

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
