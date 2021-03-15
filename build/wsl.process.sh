#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

[ -d $WSL_BUILD_PROCESS_PATH ] || mkdir -p $WSL_BUILD_PROCESS_PATH


[ -d $WSL_BUILD_PROCESS_PATH/source ] || mkdir -p $WSL_BUILD_PROCESS_PATH/source
[ -d $WSL_BUILD_PROCESS_PATH/build ] || mkdir -p $WSL_BUILD_PROCESS_PATH/build
[ -d $WSL_BUILD_PROCESS_PATH/build/source ] || mkdir -p $WSL_BUILD_PROCESS_PATH/build/source
[ -d $WSL_BUILD_PROCESS_PATH/release ] || mkdir -p $WSL_BUILD_PROCESS_PATH/release
[ -d $WSL_BUILD_PROCESS_PATH/test ] || mkdir -p $WSL_BUILD_PROCESS_PATH/test

cp -rfu ./source/* $WSL_BUILD_PROCESS_PATH/source/
cp -rfu ./build/ubuntu*.sh $WSL_BUILD_PROCESS_PATH/build/
cp -rfu ./build/source/* $WSL_BUILD_PROCESS_PATH/build/source/
cp -rfu ./release/*.csv $WSL_BUILD_PROCESS_PATH/release/
cp -rfu ./test/* $WSL_BUILD_PROCESS_PATH/test/
cp -rfu ./version.ini $WSL_BUILD_PROCESS_PATH/

WSL_POPD=$PWD
cd $WSL_BUILD_PROCESS_PATH

chmod -R -x+X source

/bin/sh -- ./build/ubuntu.sh $1
RETV=$?

cd $WSL_POPD

if [ "$1" = "clean" ]; then
	rm -rf $WSL_BUILD_PROCESS_PATH
fi

if [ "$1" = "release" ]; then
	[ -d ./release ] || mkdir -p ./release 
	cp -rfu $WSL_BUILD_PROCESS_PATH/release/*.7z ./release/
	cp -rfu $WSL_BUILD_PROCESS_PATH/release/*.csv ./release/
fi

if [ "$RETV" = "1" ]; then
	exit 1
fi
exit 0
