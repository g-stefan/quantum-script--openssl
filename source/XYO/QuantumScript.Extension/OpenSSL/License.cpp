// Quantum Script Extension OpenSSL
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/OpenSSL/License.hpp>

namespace XYO::QuantumScript::Extension::OpenSSL::License {

	const char *license() {
		return 
		#include <XYO/QuantumScript.Extension/OpenSSL/License.quantum-script--openssl.Source.cpp>
		"\r\n"
		"bzip2\r\n"
		"\r\n"
		#include <XYO/QuantumScript.Extension/OpenSSL/License.bzip2.Source.cpp>
		"\r\n"
		"libxml2\r\n"
		"\r\n"
		#include <XYO/QuantumScript.Extension/OpenSSL/License.libxml2.Source.cpp>
		"\r\n"
		"libxlt\r\n"
		"\r\n"
		#include <XYO/QuantumScript.Extension/OpenSSL/License.libxslt.Source.cpp>
		"\r\n"
		"openssl\r\n"
		"\r\n"
		#include <XYO/QuantumScript.Extension/OpenSSL/License.openssl.Source.cpp>
		;
	};

	const char *shortLicense() {
		return XYO::ManagedMemory::License::shortLicense();
	};

};
