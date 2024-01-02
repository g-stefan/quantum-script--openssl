// Quantum Script Extension OpenSSL
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/OpenSSL/License.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Copyright.hpp>

namespace XYO::QuantumScript::Extension::OpenSSL::License {

	std::string license() {
		std::string retV;
		retV += ManagedMemory::License::licenseMITHeader();
		retV += Copyright::copyright();
		retV += "\r\n";
		retV += ManagedMemory::License::licenseMITContent();

		retV +=
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

		return retV;
	};

	std::string shortLicense() {
		return XYO::ManagedMemory::License::shortLicense();
	};

};
