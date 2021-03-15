#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

if ! [ "$1" = "" ]; then
	if [ -f "./build/$1.sh" ]; then
		. ./build/$1.sh $2
		RETV=$?
		if [ "$RETV" = "1" ]; then
			exit 1
		fi
		exit 0
	fi
fi
DEFAULT_PLATFORM=
if [ "$XYO_PLATFORM" = "ubuntu-18" ]; then
	DEFAULT_PLATFORM=ubuntu
fi
if [ "$XYO_PLATFORM" = "ubuntu-20" ]; then
	DEFAULT_PLATFORM=ubuntu
fi
if [ "$XYO_PLATFORM" = "mingw32" ]; then
	DEFAULT_PLATFORM=mingw32
fi
if [ "$XYO_PLATFORM" = "mingw64" ]; then
	DEFAULT_PLATFORM=mingw64
fi
if [ "$DEFAULT_PLATFORM" = "" ]; then
	if [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
                DEFAULT_PLATFORM=ubuntu
        fi
	if [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ]; then
		DEFAULT_PLATFORM=mingw32
	fi
	if [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
		DEFAULT_PLATFORM=mingw64
	fi
fi
if [ "$DEFAULT_PLATFORM" = "" ]; then
	echo "build [platform] [mode]"
	exit 0
fi	
. ./build/$DEFAULT_PLATFORM.sh $1
RETV=$?
if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
