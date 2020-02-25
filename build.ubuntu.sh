#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

if [ -f ../build.ubuntu.config.sh ]; then
	. ../build.ubuntu.config.sh
fi

RESTORE_PATH=$PATH
RESTORE_LD_LIBRARY_PATH=$LD_LIBRARY_PATH

if [ "$XYO_PATH_REPOSITORY" = "" ]; then
	PATH_REPOSITORY=../repository
	if [ -f ../../sdk.build.ubuntu.sh ]; then
		PATH_REPOSITORY=../../repository
	fi
	POPD=$PWD
	[ -d $PATH_REPOSITORY ] || mkdir -p $PATH_REPOSITORY
	cd $PATH_REPOSITORY
	XYO_PATH_REPOSITORY=$PWD
	cd $POPD
fi

if [ "$XYO_PATH_RELEASE" = "" ]; then
	PATH_RELEASE=../release
	if [ -f ../../sdk.build.ubuntu.sh ]; then
		PATH_RELEASE=../../release
	fi
	POPD=$PWD
	[ -d $PATH_RELEASE ] || mkdir -p $PATH_RELEASE
	cd $PATH_RELEASE
	XYO_PATH_RELEASE=$PWD
	cd $POPD
fi

PATH=$XYO_PATH_REPOSITORY/bin:$PATH
LD_LIBRARY_PATH=$XYO_PATH_REPOSITORY/bin:$LD_LIBRARY_PATH

if [ -d "bin" ]; then
	POPD=$PWD
	cd bin
	PATH=$PWD:$PATH
	LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
	cd $POPD
fi

export LD_LIBRARY_PATH
export XYO_PATH_REPOSITORY
export XYO_PATH_RELEASE

RETV=0

if [ "$1" = "" ]; then
	. ./build.ubuntu.make.sh make
else
	if [ -f "build.ubuntu.$1.sh" ]; then
		. ./build.ubuntu.$1.sh
	else
		. ./build.ubuntu.make.sh $1
	fi
fi

RETV=$?

PATH=$RESTORE_PATH
LD_LIBRARY_PATH=$RESTORE_LD_LIBRARY_PATH
export LD_LIBRARY_PATH

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
