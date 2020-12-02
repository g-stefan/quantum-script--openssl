#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

echo "-> analyze quantum-script-extension-openssl"

cmdX(){
	echo "-> $@"
	if ! "$@" ; then
		echo "Error: $@"
		exit 1
	fi
}

cmdX scan-build ./port/build.ubuntu.make.sh
