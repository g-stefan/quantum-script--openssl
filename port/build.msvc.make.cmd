@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

set ACTION=%1
if "%1" == "" set ACTION=make

echo -^> %ACTION% quantum-script-extension-openssl

goto StepX
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: %ACTION%"
exit 1
:StepX

call :cmdX file-to-cs --touch=source/quantum-script-extension-openssl.cpp --file-in=source/quantum-script-extension-openssl.js --file-out=source/quantum-script-extension-openssl.src --is-string --name=extensionOpenSSLSource
call :cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-bzip2.txt --file-out=source/license-dependency-bzip2.src --is-string-direct
call :cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-libxml2.txt --file-out=source/license-dependency-libxml2.src --is-string-direct
call :cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-libxslt.txt --file-out=source/license-dependency-libxslt.src --is-string-direct
call :cmdX file-to-cs --touch=source/quantum-script-extension-openssl-license.cpp --file-in=source/license-dependency-openssl.txt --file-out=source/license-dependency-openssl.src --is-string-direct
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-openssl.static.compile
call :cmdX xyo-cc --mode=%ACTION% @util/quantum-script-extension-openssl.dynamic.compile
