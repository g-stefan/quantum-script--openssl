// Quantum Script Extension OpenSSL
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
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
