#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

export XYO_PATH_REPOSITORY=$HOME/SDK32
export XYO_PATH_RELEASE=$HOME/SDK32/release
export XYO_PATH_REPOSITORY_LIBRARY=
export PATH=$PATH:/c/msys64/mingw32/bin/../libexec;

/bin/sh -- ./port/build.ubuntu.sh $1
RETV=$?

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
