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
	elif [ -f "./build/platform/$1.sh" ]; then
		. ./build/platform/$1.sh $2
		RETV=$?
		if [ "$RETV" = "1" ]; then
			exit 1
		fi
		exit 0
	fi
fi

. ./build/platform/platform.select.sh

if [ "$XYO_PLATFORM" = "" ]; then
	echo "build [platform] [mode]"
	exit 0
fi

if ! [ -f "./build/platform/$XYO_PLATFORM.sh" ]; then
	echo "build [platform] [mode]"
	exit 0
fi

. ./build/platform/$XYO_PLATFORM.sh $1
RETV=$?
if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
