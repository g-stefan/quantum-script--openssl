#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

if ! [ "$XYO_PLATFORM" = "" ]; then
	return 0
fi

if [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
	export XYO_PLATFORM="ubuntu"
	if [ -f "/etc/lsb-release" ]; then
		RELEASE=`cat /etc/lsb-release | grep DISTRIB_RELEASE| cut -d "=" -f 2`
		export XYO_PLATFORM="ubuntu-$RELEASE"
	fi
	if command -v termux-setup-storage >/dev/null; then
		export XYO_PLATFORM="termux"
	fi
fi

if [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ]; then
	export XYO_PLATFORM="mingw32"
fi

if [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
	export XYO_PLATFORM="mingw64"
fi
