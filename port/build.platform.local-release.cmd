@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

echo -^> platform local-release quantum-script-extension-openssl

goto cmdXDefined
:cmdX
%*
if errorlevel 1 goto cmdXError
goto :eof
:cmdXError
echo "Error: local-release"
exit 1
:cmdXDefined

call :cmdX cmd.exe /C "port\build.msvc64.cmd clean"
call :cmdX cmd.exe /C "port\build.msvc64.cmd local-release"
call :cmdX cmd.exe /C "port\build.msvc64.cmd clean"

call :cmdX cmd.exe /C "port\build.msvc32.cmd clean"
call :cmdX cmd.exe /C "port\build.msvc32.cmd local-release"
call :cmdX cmd.exe /C "port\build.msvc32.cmd clean"

call :cmdX cmd.exe /C "port\build.mingw64.cmd clean"
call :cmdX cmd.exe /C "port\build.mingw64.cmd local-release"
call :cmdX cmd.exe /C "port\build.mingw64.cmd clean"

call :cmdX cmd.exe /C "port\build.mingw32.cmd clean"
call :cmdX cmd.exe /C "port\build.mingw32.cmd local-release"
call :cmdX cmd.exe /C "port\build.mingw32.cmd clean"

call :cmdX cmd.exe /C "port\build.wsl-ubuntu-20.04.cmd clean"
call :cmdX cmd.exe /C "port\build.wsl-ubuntu-20.04.cmd local-release"
call :cmdX cmd.exe /C "port\build.wsl-ubuntu-20.04.cmd clean"

call :cmdX cmd.exe /C "port\build.wsl-ubuntu-18.04.cmd clean"
call :cmdX cmd.exe /C "port\build.wsl-ubuntu-18.04.cmd local-release"
call :cmdX cmd.exe /C "port\build.wsl-ubuntu-18.04.cmd clean"
