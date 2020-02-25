//
// Quantum Script Extension OpenSSL
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL_HPP
#define QUANTUM_SCRIPT_EXTENSION_OPENSSL_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL__EXPORT_HPP
#include "quantum-script-extension-openssl--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL_COPYRIGHT_HPP
#include "quantum-script-extension-openssl-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL_LICENSE_HPP
#include "quantum-script-extension-openssl-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_OPENSSL_VERSION_HPP
#include "quantum-script-extension-openssl-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace OpenSSL {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT void registerInternalExtension(Executive *executive);

				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool privateEncrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::Buffer &output);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool privateDecrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::Buffer &output);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool publicEncrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::Buffer &output);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool publicDecrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::Buffer &output);

				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool privateEncryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool privateDecryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool publicEncryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);
				QUANTUM_SCRIPT_EXTENSION_OPENSSL_EXPORT bool publicDecryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);

			};
		};
	};
};

#endif

