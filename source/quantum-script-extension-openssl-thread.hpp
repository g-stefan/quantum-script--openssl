//
// Quantum Script Extension OpenSSL
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL_THREAD_HPP
#define QUANTUM_SCRIPT_EXTENSION_OPENSSL_THREAD_HPP

#ifndef QUANTUM_SCRIPT_HPP
#	include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL__EXPORT_HPP
#	include "quantum-script-extension-openssl--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace OpenSSL {

				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool threadLockInit();

			};
		};
	};
};

#endif
