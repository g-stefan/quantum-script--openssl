// Quantum Script Extension OpenSSL
// Copyright (c) 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2023 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/OpenSSL/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::OpenSSL {

	XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT void registerInternalExtension(Executive *executive);

XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool privateEncrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool privateDecrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool publicEncrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool publicDecrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);

				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool privateEncryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool privateDecryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool publicEncryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);
				XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT bool publicDecryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);
};

#endif
