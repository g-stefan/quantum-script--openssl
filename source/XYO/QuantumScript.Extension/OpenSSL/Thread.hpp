// Quantum Script Extension OpenSSL
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_THREAD_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_THREAD_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/OpenSSL/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::OpenSSL {

	XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool threadLockInit();

};

#endif
