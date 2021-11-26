#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

export XYO_PLATFORM="ubuntu-20.04"
export XYO_PATH_REPOSITORY=$HOME/SDK
export XYO_PATH_RELEASE=$HOME/SDK/release

/bin/sh -- ./build/platform/ubuntu.sh $1
RETV=$?

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
