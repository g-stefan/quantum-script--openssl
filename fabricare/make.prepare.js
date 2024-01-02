// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("make.prepare");

runInPath("source/XYO/QuantumScript.Extension/OpenSSL",function(){
	
	exitIf(fileToCS("--touch=License.cpp", "--file-in=License/bzip2.txt", "--file-out=License.bzip2.Source.cpp", "--is-string-direct"));
	exitIf(fileToCS("--touch=License.cpp", "--file-in=License/libxml2.txt", "--file-out=License.libxml2.Source.cpp", "--is-string-direct"));
	exitIf(fileToCS("--touch=License.cpp", "--file-in=License/libxslt.txt", "--file-out=License.libxslt.Source.cpp", "--is-string-direct"));
	exitIf(fileToCS("--touch=License.cpp", "--file-in=License/openssl.txt", "--file-out=License.openssl.Source.cpp", "--is-string-direct"));

	exitIf(fileToCS("--touch=Library.cpp","--file-in=Library.js","--file-out=Library.Source.cpp","--is-string","--name=librarySource"));

});
