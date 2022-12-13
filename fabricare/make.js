// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("make");

exitIf(fileToCS("--touch=source/XYO/QuantumScript.Extension/OpenSSL/License.cpp", "--file-in=source/XYO/QuantumScript.Extension/OpenSSL/License/quantum-script--openssl.txt", "--file-out=source/XYO/QuantumScript.Extension/OpenSSL/License.quantum-script--openssl.Source.cpp", "--is-string-direct"));
exitIf(fileToCS("--touch=source/XYO/QuantumScript.Extension/OpenSSL/License.cpp", "--file-in=source/XYO/QuantumScript.Extension/OpenSSL/License/bzip2.txt", "--file-out=source/XYO/QuantumScript.Extension/OpenSSL/License.bzip2.Source.cpp", "--is-string-direct"));
exitIf(fileToCS("--touch=source/XYO/QuantumScript.Extension/OpenSSL/License.cpp", "--file-in=source/XYO/QuantumScript.Extension/OpenSSL/License/libxml2.txt", "--file-out=source/XYO/QuantumScript.Extension/OpenSSL/License.libxml2.Source.cpp", "--is-string-direct"));
exitIf(fileToCS("--touch=source/XYO/QuantumScript.Extension/OpenSSL/License.cpp", "--file-in=source/XYO/QuantumScript.Extension/OpenSSL/License/libxslt.txt", "--file-out=source/XYO/QuantumScript.Extension/OpenSSL/License.libxslt.Source.cpp", "--is-string-direct"));
exitIf(fileToCS("--touch=source/XYO/QuantumScript.Extension/OpenSSL/License.cpp", "--file-in=source/XYO/QuantumScript.Extension/OpenSSL/License/openssl.txt", "--file-out=source/XYO/QuantumScript.Extension/OpenSSL/License.openssl.Source.cpp", "--is-string-direct"));

runInPath("source/XYO/QuantumScript.Extension/OpenSSL",function(){
	exitIf(fileToCS("--touch=Library.cpp","--file-in=Library.js","--file-out=Library.Source.cpp","--is-string","--name=librarySource"));
});

if (!Fabricare.include("make." + Project.make)) {
	messageError("Don't know how to make '" + Project.make + "'!");
	exit(1);
};
