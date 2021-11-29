#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

[ -d $WSL_BUILD_PROCESS_PATH ] || mkdir -p $WSL_BUILD_PROCESS_PATH

[ -d $WSL_BUILD_PROCESS_PATH/source ] || mkdir -p $WSL_BUILD_PROCESS_PATH/source
[ -d $WSL_BUILD_PROCESS_PATH/build ] || mkdir -p $WSL_BUILD_PROCESS_PATH/build
[ -d $WSL_BUILD_PROCESS_PATH/build/source ] || mkdir -p $WSL_BUILD_PROCESS_PATH/build/source
[ -d $WSL_BUILD_PROCESS_PATH/build/platform ] || mkdir -p $WSL_BUILD_PROCESS_PATH/build/platform
[ -d $WSL_BUILD_PROCESS_PATH/release ] || mkdir -p $WSL_BUILD_PROCESS_PATH/release
[ -d $WSL_BUILD_PROCESS_PATH/test ] || mkdir -p $WSL_BUILD_PROCESS_PATH/test

cp -rfu ./build/ubuntu.* $WSL_BUILD_PROCESS_PATH/build/
[ -d ./source/ ] && cp -rfu ./source/* $WSL_BUILD_PROCESS_PATH/source/
[ -d ./build/source/ ] && cp -rfu ./build/source/* $WSL_BUILD_PROCESS_PATH/build/source/
[ -d ./build/platform/ ] && cp -rfu ./build/platform/* $WSL_BUILD_PROCESS_PATH/build/platform/
[ -d ./release/ ] && cp -rfu ./release/*${XYO_PLATFORM}.7z $WSL_BUILD_PROCESS_PATH/release/
[ -d ./release/ ] && cp -rfu ./release/*.csv $WSL_BUILD_PROCESS_PATH/release/
[ -d ./test/ ] && cp -rfu ./test/* $WSL_BUILD_PROCESS_PATH/test/
[ -f ./version.ini ] && cp -rfu ./version.ini $WSL_BUILD_PROCESS_PATH/

WSL_POPD=$PWD
cd $WSL_BUILD_PROCESS_PATH

chmod -R -x+X source

/bin/sh -- ./build/platform/ubuntu.sh $1
RETV=$?

cd $WSL_POPD

if [ "$1" = "clean" ]; then
	rm -rf $WSL_BUILD_PROCESS_PATH
fi

if [ "$1" = "release" ]; then
	[ -d ./release ] || mkdir -p ./release 
	if ls $WSL_BUILD_PROCESS_PATH/release/*.7z 1> /dev/null 2>&1; then
		cp -rfu $WSL_BUILD_PROCESS_PATH/release/*.7z ./release/
	fi
	if ls $WSL_BUILD_PROCESS_PATH/release/*.csv 1> /dev/null 2>&1; then
		cp -rfu $WSL_BUILD_PROCESS_PATH/release/*.csv ./release/
	fi
fi

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
