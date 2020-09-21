#!/bin/sh
# Public domain
# http://unlicense.org/
# Created by Grigore Stefan <g_stefan@yahoo.com>

ACTION=$1
if [ "$ACTION" = "" ]; then
	ACTION=make
fi

echo "-> $ACTION quantum-script-extension-openssl"

cmdX(){
	if ! "$@" ; then
		echo "Error: $ACTION"
		exit 1
	fi
}

cmdX file-to-cs --touch=source/quantum-script-extension-openssl.cpp --file-in=source/quantum-script-extension-openssl.js --file-out=source/quantum-script-extension-openssl.src --is-string --name=extensionOpenSSLSource
cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-bzip2.txt --file-out=source/license-dependency-bzip2.src --is-string-direct
cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-libxml2.txt --file-out=source/license-dependency-libxml2.src --is-string-direct
cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-libxslt.txt --file-out=source/license-dependency-libxslt.src --is-string-direct
cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-openssl.txt --file-out=source/license-dependency-openssl.src --is-string-direct
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-openssl.static.compile.unix
cmdX xyo-cc --mode=$ACTION @util/quantum-script-extension-openssl.dynamic.compile
