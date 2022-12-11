// Quantum Script Extension OpenSSL
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

#include <XYO/Cryptography.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Library.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Copyright.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/License.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Version.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Library.Source.cpp>
#include <XYO/QuantumScript.Extension/OpenSSL/Thread.hpp>
#include <XYO/QuantumScript.Extension/Buffer/VariableBuffer.hpp>

namespace XYO::QuantumScript::Extension::OpenSSL {
	typedef Extension::Buffer::VariableBuffer VariableBuffer;

	static void ctxDelete(void *ssl_ctx) {
					SSL_CTX_free((SSL_CTX *)ssl_ctx);
				};

				static void sslDelete(void *ssl) {
					SSL_free((SSL *)ssl);
				};

				static void bioDelete(void *bio) {
					BIO_free_all((BIO *)bio);
				};

				static void rsaDelete(void *rsa) {
					RSA_free((RSA *)rsa);
				};

				static TPointer<Variable> ctxNew(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-ctx-new\n");
#endif

					SSL_CTX *ssl_ctx;
					ssl_ctx = SSL_CTX_new(SSLv23_client_method());
					SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);

					return VariableResource::newVariable(ssl_ctx, (VariableResource::ResourceDelete)ctxDelete);
				};

				static TPointer<Variable> sslNew(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-ssl-new\n");
#endif

					TPointerX<Variable> &ctxSSL = arguments->index(0);

					SSL *ssl;
					ssl = SSL_new((SSL_CTX *)(((VariableResource *)ctxSSL.value())->resource));

					return VariableResource::newVariable(ssl, (VariableResource::ResourceDelete)sslDelete);
				};

				static TPointer<Variable> bioConnect(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-connect\n");
#endif

					TPointerX<Variable> &ctxSSL = arguments->index(0);
					String targetServer = (arguments->index(1))->toString();

					BIO *bio;
					SSL *ssl;

					bio = BIO_new_ssl_connect((SSL_CTX *)(((VariableResource *)ctxSSL.value())->resource));
					BIO_get_ssl(bio, &ssl);
					if (!ssl) {
						BIO_free_all(bio);
						return Context::getValueUndefined();
					};
					BIO_set_conn_hostname(bio, targetServer.value());
					if (SSL_do_handshake(ssl) <= 0) {
						BIO_free_all(bio);
						return Context::getValueUndefined();
					};

					return VariableResource::newVariable(bio, (VariableResource::ResourceDelete)bioDelete);
				};

				static TPointer<Variable> bioClose(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-close\n");
#endif

					TPointerX<Variable> &bio = arguments->index(0);

					BIO_ssl_shutdown((BIO *)(((VariableResource *)bio.value())->resource));

					return Context::getValueUndefined();
				};

				static TPointer<Variable> bioRead(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-read\n");
#endif
					String retV(1024, 1024); // first 1024, next + 1024 bytes
					Number ln;
					size_t readLn;
					size_t readToLn;
					size_t readX;
					size_t readTotal;
					size_t k;
					char buffer[1024];

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					ln = (arguments->index(1))->toNumber();
					if (isnan(ln) || isinf(ln) || signbit(ln)) {
						return Context::getValueUndefined();
					};

					readToLn = (size_t)(ln);
					readTotal = 0;
					readX = 1024;
					if (readToLn < readX) {
						readX = readToLn;
					};
					for (;;) {
						readLn = BIO_read(bio, buffer, readX);

						if (readLn > 0) {
							retV.concatenate(buffer, readLn);
						};
						// end of file
						if (readLn < readX) {
							break;
						};
						// end of read
						readTotal += readLn;
						if (readTotal >= readToLn) {
							break;
						};
						readX = readToLn - readTotal;
						if (readX > 1024) {
							readX = 1024;
						};
					};

					return VariableString::newVariable(retV);
				};

				static TPointer<Variable> bioReadLn(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-read-ln\n");
#endif
					String retV(1024, 1024); // first 1024, next + 1024 bytes
					Number ln;
					size_t readLn;
					size_t readToLn;
					size_t readTotal;
					size_t k;
					char buffer[2];

					buffer[1] = 0;

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					ln = (arguments->index(1))->toNumber();
					if (isnan(ln) || isinf(ln) || signbit(ln)) {
						return Context::getValueUndefined();
					};

					readToLn = (size_t)(ln);
					readTotal = 0;
					if (readToLn < 1) {
						return VariableString::newVariable("");
					};
					for (;;) {
						readLn = BIO_read(bio, buffer, 1);
						if (readLn > 0) {

							if (buffer[0] == '\r') {
								if (readTotal + 1 >= readToLn) {
									retV.concatenate("\r", 1);
									return VariableString::newVariable(retV);
								};

								readLn = BIO_read(bio, buffer, 1);
								if (readLn > 0) {
									if (buffer[0] == '\n') {
										retV.concatenate("\r", 1);
										retV.concatenate("\n", 1);
										return VariableString::newVariable(retV);
										break;
									};
									retV.concatenate(buffer, 1);
									readTotal += 2;
									if (readTotal >= readToLn) {
										return VariableString::newVariable(retV);
									};
									continue;
								};

								retV.concatenate("\r", 1);
								// end of file
								return VariableString::newVariable(retV);
							};

							retV.concatenate(buffer, 1);
							readTotal++;
							if (readTotal >= readToLn) {
								return VariableString::newVariable(retV);
							};
							continue;
						};
						// connection interrupted - 0 to read ...
						if (readTotal == 0) {
							break;
						};
						// end of file
						return VariableString::newVariable(retV);
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> bioWrite(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-write\n");
#endif

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					String toWrite = (arguments->index(1))->toString();
					return VariableNumber::newVariable((Number)(BIO_write(bio,
					                                                      toWrite.value(), toWrite.length())));
				};

				static TPointer<Variable> bioWriteLn(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-write-ln\n");
#endif

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					String toWrite = (arguments->index(1))->toString();
					toWrite << "\r\n";
					return VariableNumber::newVariable((Number)(BIO_write(bio,
					                                                      toWrite.value(), toWrite.length())));
				};

				static TPointer<Variable> bioReadToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-read-to-buffer\n");
#endif
					size_t readLn;
					size_t readToLn;
					size_t readX;
					size_t readTotal;
					size_t k;
					Number ln;

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					TPointerX<Variable> &buffer(arguments->index(1));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					ln = (arguments->index(2))->toNumber();
					if (isnan(ln) || signbit(ln) || ln == 0.0) {
						((VariableBuffer *)buffer.value())->buffer.length = 0;
						return VariableNumber::newVariable(0);
					};
					if (isinf(ln)) {
						ln = ((VariableBuffer *)buffer.value())->buffer.size;
					};

					if (ln > ((VariableBuffer *)buffer.value())->buffer.size) {
						ln = ((VariableBuffer *)buffer.value())->buffer.size;
					};

					readToLn = (size_t)ln;
					readTotal = 0;
					readX = readToLn;
					for (;;) {
						readLn = BIO_read(bio, &(((VariableBuffer *)buffer.value())->buffer.buffer)[readTotal], readX);
						// end of transmision
						if (readLn == 0) {
							break;
						};
						readTotal += readLn;
						if (readTotal >= readToLn) {
							break;
						};
						readX = readToLn - readTotal;
					};
					((VariableBuffer *)buffer.value())->buffer.length = readTotal;
					return VariableNumber::newVariable(readTotal);
				};

				static TPointer<Variable> bioWriteFromBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-bio-write-from-buffer\n");
#endif

					BIO *bio = (BIO *)((VariableResource *)((arguments->index(0)).value()))->resource;

					TPointerX<Variable> &buffer(arguments->index(1));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					return VariableNumber::newVariable((Number)(BIO_write(bio,
					                                                      ((VariableBuffer *)buffer.value())->buffer.buffer, ((VariableBuffer *)buffer.value())->buffer.length)));
				};

				static TPointer<Variable> sha256Hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-sha256\n");
#endif

					TPointerX<Variable> &buffer(arguments->index(0));
					size_t bufferStart = (arguments->index(1))->toIndex();
					size_t bufferLn = (arguments->index(2))->toIndex();
					TPointerX<Variable> &outHash(arguments->index(3));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outHash)) {
						throw(Error("invalid parameter"));
					};

					if (bufferLn == 0) {
						return VariableBoolean::newVariable(false);
					};

					if (bufferStart + bufferLn > ((VariableBuffer *)buffer.value())->buffer.length) {
						return VariableBoolean::newVariable(false);
					};

					if (((VariableBuffer *)outHash.value())->buffer.size < SHA256_DIGEST_LENGTH) {
						((VariableBuffer *)outHash.value())->resize(SHA256_DIGEST_LENGTH);
					};

					SHA256_CTX sha256;

					if (SHA256_Init(&sha256) == 0) {
						return VariableBoolean::newVariable(false);
					};
					if (SHA256_Update(&sha256, &((VariableBuffer *)buffer.value())->buffer.buffer[bufferStart], bufferLn) == 0) {
						return VariableBoolean::newVariable(false);
					};
					if (SHA256_Final(((VariableBuffer *)outHash.value())->buffer.buffer, &sha256) == 0) {
						return VariableBoolean::newVariable(false);
					};

					((VariableBuffer *)outHash.value())->buffer.length = SHA256_DIGEST_LENGTH;

					return VariableBoolean::newVariable(true);
				};

				static TPointer<Variable> sha512Hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-sha512\n");
#endif

					TPointerX<Variable> &buffer(arguments->index(0));
					size_t bufferStart = (arguments->index(1))->toIndex();
					size_t bufferLn = (arguments->index(2))->toIndex();
					TPointerX<Variable> &outHash(arguments->index(3));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outHash)) {
						throw(Error("invalid parameter"));
					};

					if (bufferLn == 0) {
						return VariableBoolean::newVariable(false);
					};

					if (bufferStart + bufferLn > ((VariableBuffer *)buffer.value())->buffer.length) {
						return VariableBoolean::newVariable(false);
					};

					if (((VariableBuffer *)outHash.value())->buffer.size < SHA512_DIGEST_LENGTH) {
						((VariableBuffer *)outHash.value())->resize(SHA512_DIGEST_LENGTH);
					};

					SHA512_CTX sha512;

					if (SHA512_Init(&sha512) == 0) {
						return VariableBoolean::newVariable(false);
					};
					if (SHA512_Update(&sha512, &((VariableBuffer *)buffer.value())->buffer.buffer[bufferStart], bufferLn) == 0) {
						return VariableBoolean::newVariable(false);
					};
					if (SHA512_Final(((VariableBuffer *)outHash.value())->buffer.buffer, &sha512) == 0) {
						return VariableBoolean::newVariable(false);
					};

					((VariableBuffer *)outHash.value())->buffer.length = SHA512_DIGEST_LENGTH;

					return VariableBoolean::newVariable(true);
				};

				static TPointer<Variable> rsaSize(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-size\n");
#endif
					RSA *rsa = (RSA *)((VariableResource *)((arguments->index(0)).value()))->resource;
					return VariableNumber::newVariable(RSA_size(rsa));
				};

				static TPointer<Variable> rsaPublicKey(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-public-key\n");
#endif

					TPointerX<Variable> &buffer(arguments->index(0));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					RSA *rsa = nullptr;
					BIO *bio;
					bio = BIO_new_mem_buf(((VariableBuffer *)buffer.value())->buffer.buffer, ((VariableBuffer *)buffer.value())->buffer.length);
					if (bio == nullptr) {
						return VariableNull::newVariable();
					};
					rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);

					BIO_free_all(bio);

					if (rsa == nullptr) {
						return VariableNull::newVariable();
					};

					return VariableResource::newVariable(rsa, (VariableResource::ResourceDelete)rsaDelete);
				};

				static TPointer<Variable> rsaPrivateKey(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-private-key\n");
#endif

					TPointerX<Variable> &buffer(arguments->index(0));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					RSA *rsa = nullptr;
					BIO *bio;
					bio = BIO_new_mem_buf(((VariableBuffer *)buffer.value())->buffer.buffer, ((VariableBuffer *)buffer.value())->buffer.length);
					if (bio == nullptr) {
						return VariableNull::newVariable();
					};
					rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr);

					BIO_free_all(bio);

					if (rsa == nullptr) {
						return VariableNull::newVariable();
					};

					return VariableResource::newVariable(rsa, (VariableResource::ResourceDelete)rsaDelete);
				};

				static TPointer<Variable> rsaPublicEncrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-public-encrypt\n");
#endif

					RSA *rsa = (RSA *)((VariableResource *)((arguments->index(0)).value()))->resource;
					TPointerX<Variable> &buffer(arguments->index(1));
					TPointerX<Variable> &outBuffer(arguments->index(2));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outBuffer)) {
						throw(Error("invalid parameter"));
					};

					size_t outputSize = RSA_size(rsa);

					if (((VariableBuffer *)outBuffer.value())->buffer.size < outputSize) {
						((VariableBuffer *)outBuffer.value())->resize(outputSize);
					};

					if (((VariableBuffer *)buffer.value())->buffer.length == 0) {
						throw(Error("invalid length"));
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);

					if (RSA_public_encrypt(
					        ((VariableBuffer *)buffer.value())->buffer.length,
					        ((VariableBuffer *)buffer.value())->buffer.buffer,
					        ((VariableBuffer *)outBuffer.value())->buffer.buffer,
					        rsa, RSA_PKCS1_PADDING) == outputSize) {
						((VariableBuffer *)outBuffer.value())->buffer.length = outputSize;
						return VariableBoolean::newVariable(true);
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);
					((VariableBuffer *)outBuffer.value())->buffer.length = 0;
					return VariableBoolean::newVariable(false);
				};

				static TPointer<Variable> rsaPublicDecrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-public-decrypt\n");
#endif

					RSA *rsa = (RSA *)((VariableResource *)((arguments->index(0)).value()))->resource;
					TPointerX<Variable> &buffer(arguments->index(1));
					TPointerX<Variable> &outBuffer(arguments->index(2));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outBuffer)) {
						throw(Error("invalid parameter"));
					};

					size_t outputSize = RSA_size(rsa);

					if (((VariableBuffer *)outBuffer.value())->buffer.size < outputSize) {
						((VariableBuffer *)outBuffer.value())->resize(outputSize);
					};

					if (((VariableBuffer *)buffer.value())->buffer.length == 0) {
						throw(Error("invalid length"));
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);

					int newLn = RSA_public_decrypt(
					    ((VariableBuffer *)buffer.value())->buffer.length,
					    ((VariableBuffer *)buffer.value())->buffer.buffer,
					    ((VariableBuffer *)outBuffer.value())->buffer.buffer,
					    rsa, RSA_PKCS1_PADDING);

					if (newLn > 0) {
						((VariableBuffer *)outBuffer.value())->buffer.length = newLn;
						return VariableBoolean::newVariable(true);
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);
					((VariableBuffer *)outBuffer.value())->buffer.length = 0;
					return VariableBoolean::newVariable(false);
				};

				static TPointer<Variable> rsaPrivateEncrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-private-encrypt\n");
#endif

					RSA *rsa = (RSA *)((VariableResource *)((arguments->index(0)).value()))->resource;
					TPointerX<Variable> &buffer(arguments->index(1));
					TPointerX<Variable> &outBuffer(arguments->index(2));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outBuffer)) {
						throw(Error("invalid parameter"));
					};

					size_t outputSize = RSA_size(rsa);

					if (((VariableBuffer *)outBuffer.value())->buffer.size < outputSize) {
						((VariableBuffer *)outBuffer.value())->resize(outputSize);
					};

					if (((VariableBuffer *)buffer.value())->buffer.length == 0) {
						throw(Error("invalid length"));
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);

					if (RSA_private_encrypt(
					        ((VariableBuffer *)buffer.value())->buffer.length,
					        ((VariableBuffer *)buffer.value())->buffer.buffer,
					        ((VariableBuffer *)outBuffer.value())->buffer.buffer,
					        rsa, RSA_PKCS1_PADDING) == outputSize) {
						((VariableBuffer *)outBuffer.value())->buffer.length = outputSize;
						return VariableBoolean::newVariable(true);
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);
					((VariableBuffer *)outBuffer.value())->buffer.length = 0;
					return VariableBoolean::newVariable(false);
				};

				static TPointer<Variable> rsaPrivateDecrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_VM_DEBUG_RUNTIME
					printf("- openssl-rsa-private-decrypt\n");
#endif
					RSA *rsa = (RSA *)((VariableResource *)((arguments->index(0)).value()))->resource;
					TPointerX<Variable> &buffer(arguments->index(1));
					TPointerX<Variable> &outBuffer(arguments->index(2));

					if (!TIsType<VariableBuffer>(buffer)) {
						throw(Error("invalid parameter"));
					};

					if (!TIsType<VariableBuffer>(outBuffer)) {
						throw(Error("invalid parameter"));
					};

					size_t outputSize = RSA_size(rsa);

					if (((VariableBuffer *)outBuffer.value())->buffer.size < outputSize) {
						((VariableBuffer *)outBuffer.value())->resize(outputSize);
					};

					if (((VariableBuffer *)buffer.value())->buffer.length == 0) {
						throw(Error("invalid length"));
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);

					int newLn = RSA_private_decrypt(
					    ((VariableBuffer *)buffer.value())->buffer.length,
					    ((VariableBuffer *)buffer.value())->buffer.buffer,
					    ((VariableBuffer *)outBuffer.value())->buffer.buffer,
					    rsa, RSA_PKCS1_PADDING);

					if (newLn > 0) {
						((VariableBuffer *)outBuffer.value())->buffer.length = newLn;
						return VariableBoolean::newVariable(true);
					};

					memset(((VariableBuffer *)outBuffer.value())->buffer.buffer, 0, ((VariableBuffer *)outBuffer.value())->buffer.size);
					((VariableBuffer *)outBuffer.value())->buffer.length = 0;
					return VariableBoolean::newVariable(false);
				};

				static TPointer<Variable> cryptPrivateEncrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-private-encrypt\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					TPointerX<Variable> &dataV = arguments->index(1);

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;
					const uint8_t *data = nullptr;
					size_t dataSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (TIsType<VariableString>(dataV)) {
						data = (const uint8_t *)(((VariableString *)dataV.value())->value.value());
						dataSize = ((VariableString *)dataV.value())->value.length();
					} else if (TIsType<VariableBuffer>(dataV)) {
						data = (const uint8_t *)(((VariableBuffer *)dataV.value())->buffer.buffer);
						dataSize = ((VariableBuffer *)dataV.value())->buffer.length;
					};

					if (password != nullptr) {
						if (data != nullptr) {
							XYO::System::Buffer output;
							if (privateEncrypt(password, passwordSize, data, dataSize, output)) {
								return VariableBuffer::newVariable(output.buffer, output.length);
							};
						};
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPrivateDecrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-private-decrypt\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					TPointerX<Variable> &dataV = arguments->index(1);

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;
					const uint8_t *data = nullptr;
					size_t dataSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (TIsType<VariableString>(dataV)) {
						data = (const uint8_t *)(((VariableString *)dataV.value())->value.value());
						dataSize = ((VariableString *)dataV.value())->value.length();
					} else if (TIsType<VariableBuffer>(dataV)) {
						data = (const uint8_t *)(((VariableBuffer *)dataV.value())->buffer.buffer);
						dataSize = ((VariableBuffer *)dataV.value())->buffer.length;
					};

					if (password != nullptr) {
						if (data != nullptr) {
							XYO::System::Buffer output;
							if (privateDecrypt(password, passwordSize, data, dataSize, output)) {
								return VariableBuffer::newVariable(output.buffer, output.length);
							};
						};
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPublicEncrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-public-encrypt\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					TPointerX<Variable> &dataV = arguments->index(1);

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;
					const uint8_t *data = nullptr;
					size_t dataSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (TIsType<VariableString>(dataV)) {
						data = (const uint8_t *)(((VariableString *)dataV.value())->value.value());
						dataSize = ((VariableString *)dataV.value())->value.length();
					} else if (TIsType<VariableBuffer>(dataV)) {
						data = (const uint8_t *)(((VariableBuffer *)dataV.value())->buffer.buffer);
						dataSize = ((VariableBuffer *)dataV.value())->buffer.length;
					};

					if (password != nullptr) {
						if (data != nullptr) {
							XYO::System::Buffer output;
							if (publicEncrypt(password, passwordSize, data, dataSize, output)) {
								return VariableBuffer::newVariable(output.buffer, output.length);
							};
						};
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPublicDecrypt(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-public-decrypt\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					TPointerX<Variable> &dataV = arguments->index(1);

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;
					const uint8_t *data = nullptr;
					size_t dataSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (TIsType<VariableString>(dataV)) {
						data = (const uint8_t *)(((VariableString *)dataV.value())->value.value());
						dataSize = ((VariableString *)dataV.value())->value.length();
					} else if (TIsType<VariableBuffer>(dataV)) {
						data = (const uint8_t *)(((VariableBuffer *)dataV.value())->buffer.buffer);
						dataSize = ((VariableBuffer *)dataV.value())->buffer.length;
					};

					if (password != nullptr) {
						if (data != nullptr) {
							XYO::System::Buffer output;
							if (publicDecrypt(password, passwordSize, data, dataSize, output)) {
								return VariableBuffer::newVariable(output.buffer, output.length);
							};
						};
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPrivateEncryptFile(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-private-encrypt-file\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					String fileIn = (arguments->index(1))->toString();
					String fileOut = (arguments->index(2))->toString();

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (password != nullptr) {
						return VariableBoolean::newVariable(privateEncryptFile(password, passwordSize, fileIn, fileOut));
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPrivateDecryptFile(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-private-decrypt-file\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					String fileIn = (arguments->index(1))->toString();
					String fileOut = (arguments->index(2))->toString();

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (password != nullptr) {
						return VariableBoolean::newVariable(privateDecryptFile(password, passwordSize, fileIn, fileOut));
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPublicEncryptFile(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-public-encrypt-file\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					String fileIn = (arguments->index(1))->toString();
					String fileOut = (arguments->index(2))->toString();

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (password != nullptr) {
						return VariableBoolean::newVariable(publicEncryptFile(password, passwordSize, fileIn, fileOut));
					};

					return Context::getValueUndefined();
				};

				static TPointer<Variable> cryptPublicDecryptFile(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- openssl-crypt-public-decrypt-file\n");
#endif
					TPointerX<Variable> &passwordV = arguments->index(0);
					String fileIn = (arguments->index(1))->toString();
					String fileOut = (arguments->index(2))->toString();

					const uint8_t *password = nullptr;
					size_t passwordSize = 0;

					if (TIsType<VariableString>(passwordV)) {
						password = (const uint8_t *)(((VariableString *)passwordV.value())->value.value());
						passwordSize = ((VariableString *)passwordV.value())->value.length();
					} else if (TIsType<VariableBuffer>(passwordV)) {
						password = (const uint8_t *)(((VariableBuffer *)passwordV.value())->buffer.buffer);
						passwordSize = ((VariableBuffer *)passwordV.value())->buffer.length;
					};

					if (password != nullptr) {
						return VariableBoolean::newVariable(publicDecryptFile(password, passwordSize, fileIn, fileOut));
					};

					return Context::getValueUndefined();
				};

	void registerInternalExtension(Executive *executive) {
		executive->registerInternalExtension("OpenSSL", initExecutive);
	};

	void initExecutive(Executive *executive, void *extensionId) {

		String info = "OpenSSL\r\n";
		info << License::shortLicense();

		executive->setExtensionName(extensionId, "OpenSSL");
		executive->setExtensionInfo(extensionId, info);
		executive->setExtensionVersion(extensionId, Extension::OpenSSL::Version::versionWithBuild());
		executive->setExtensionPublic(extensionId, true);

		if (!threadLockInit()) {
						throw Error("OpenSSL initialization");
					};

					executive->compileStringX("Script.requireExtension(\"Buffer\");");
					executive->compileStringX("Script.requireExtension(\"Crypt\");");
					//
					executive->compileStringX("var OpenSSL={};");
					executive->compileStringX("OpenSSL.sha256Length=32;");
					executive->compileStringX("OpenSSL.sha512Length=64;");
					executive->setFunction2("OpenSSL.ctxNew()", ctxNew);
					executive->setFunction2("OpenSSL.sslNew(ctx)", sslNew);
					executive->setFunction2("OpenSSL.bioConnect(ctx,server)", bioConnect);
					executive->setFunction2("OpenSSL.bioClose(bio)", bioClose);
					executive->setFunction2("OpenSSL.bioRead(bio)", bioRead);
					executive->setFunction2("OpenSSL.bioReadLn(bio)", bioReadLn);
					executive->setFunction2("OpenSSL.bioWrite(bio,str)", bioWrite);
					executive->setFunction2("OpenSSL.bioWriteLn(bio,str)", bioWriteLn);
					executive->setFunction2("OpenSSL.bioReadToBuffer(bio,buffer)", bioReadToBuffer);
					executive->setFunction2("OpenSSL.bioWriteFromBuffer(bio,buffer)", bioWriteFromBuffer);
					executive->setFunction2("OpenSSL.sha256(bufferIn,st,ln,hashOut)", sha256Hash);
					executive->setFunction2("OpenSSL.sha512(bufferIn,st,ln,hashOut)", sha512Hash);
					executive->setFunction2("OpenSSL.rsaPublicKey(buffer)", rsaPublicKey);
					executive->setFunction2("OpenSSL.rsaPrivateKey(buffer)", rsaPrivateKey);
					executive->setFunction2("OpenSSL.rsaPublicEncrypt(rsa,buffer,bufferOut)", rsaPublicEncrypt);
					executive->setFunction2("OpenSSL.rsaPublicDecrypt(rsa,buffer,bufferOut)", rsaPublicDecrypt);
					executive->setFunction2("OpenSSL.rsaPrivateEncrypt(rsa,buffer,bufferOut)", rsaPrivateEncrypt);
					executive->setFunction2("OpenSSL.rsaPrivateDecrypt(rsa,buffer,bufferOut)", rsaPrivateDecrypt);
					executive->setFunction2("OpenSSL.rsaSize(rsa)", rsaSize);
					executive->setFunction2("Crypt.privateEncrypt(privateKey,data)", cryptPrivateEncrypt);
					executive->setFunction2("Crypt.privateDecrypt(privateKey,data)", cryptPrivateDecrypt);
					executive->setFunction2("Crypt.publicEncrypt(publicKey,data)", cryptPublicEncrypt);
					executive->setFunction2("Crypt.publicDecrypt(publicKey,data)", cryptPublicDecrypt);
					executive->setFunction2("Crypt.privateEncryptFile(key,fileIn,fileOut)", cryptPrivateEncryptFile);
					executive->setFunction2("Crypt.privateDecryptFile(key,fileIn,fileOut)", cryptPrivateDecryptFile);
					executive->setFunction2("Crypt.publicEncryptFile(key,fileIn,fileOut)", cryptPublicEncryptFile);
					executive->setFunction2("Crypt.publicDecryptFile(key,fileIn,fileOut)", cryptPublicDecryptFile);

		executive->compileStringX(librarySource);
	};

bool privateEncrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output) {
					RSA *rsa = nullptr;
					BIO *bio = BIO_new_mem_buf(const_cast<uint8_t *>(privateKey), privateKeySize);
					if (bio != nullptr) {
						rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr);
						BIO_free_all(bio);
						if (rsa != nullptr) {
							size_t rsaSize = RSA_size(rsa);
							if (rsaSize < 64) {
								RSA_free((RSA *)rsa);
								return false;
							};

							uint8_t randomKey[64];
							XYO::System::Buffer rsaKey;
							rsaKey.setSize(rsaSize);

							XYO::Cryptography::SHA512 randomSalt;
							randomSalt.processInit();
							randomSalt.processU8(privateKey, privateKeySize);
							randomSalt.processU8(data, dataSize);
							UConvert::u64ToU8(DateTime::timestampInMilliseconds(), randomKey);
							randomSalt.processU8(randomKey, 8);
							randomSalt.processDone();
							randomSalt.toU8(randomKey);

							if (RSA_private_encrypt(
							        64,
							        randomKey,
							        rsaKey.buffer,
							        rsa, RSA_PKCS1_PADDING) == rsaKey.size) {

								rsaKey.length = rsaKey.size;
								RSA_free((RSA *)rsa);

								XYO::System::Buffer outputData;
								XYO::Cryptography::Crypt::encrypt(randomKey, 64, data, dataSize, outputData);
								output.setSize(rsaKey.length + outputData.length);
								output.length = rsaKey.length + outputData.length;
								memcpy(output.buffer, rsaKey.buffer, rsaKey.length);
								memcpy(&output.buffer[rsaKey.length], outputData.buffer, outputData.length);

								return true;
							};

							RSA_free((RSA *)rsa);
						};
					};

					return false;
				};

				bool privateDecrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output) {
					RSA *rsa = nullptr;
					BIO *bio = BIO_new_mem_buf(const_cast<uint8_t *>(privateKey), privateKeySize);
					if (bio != nullptr) {
						rsa = PEM_read_bio_RSAPrivateKey(bio, &rsa, nullptr, nullptr);
						BIO_free_all(bio);
						if (rsa != nullptr) {
							size_t rsaSize = RSA_size(rsa);
							if (rsaSize < 64) {
								RSA_free((RSA *)rsa);
								return false;
							};
							if (dataSize < rsaSize + 64 + 64 + 8) {
								return false;
							};

							XYO::System::Buffer randomKey;
							randomKey.setSize(rsaSize);

							int newLn = RSA_private_decrypt(
							    rsaSize,
							    data,
							    randomKey.buffer,
							    rsa, RSA_PKCS1_PADDING);

							RSA_free((RSA *)rsa);

							if (newLn != 64) {
								return false;
							};

							return XYO::Cryptography::Crypt::decrypt(randomKey.buffer, 64, &data[rsaSize], dataSize - rsaSize, output);
						};
					};

					return false;
				};

				bool publicEncrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output) {
					RSA *rsa = nullptr;
					BIO *bio = BIO_new_mem_buf(const_cast<uint8_t *>(publicKey), publicKeySize);
					if (bio != nullptr) {
						rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);
						BIO_free_all(bio);
						if (rsa != nullptr) {
							size_t rsaSize = RSA_size(rsa);
							if (rsaSize < 64) {
								RSA_free((RSA *)rsa);
								return false;
							};

							uint8_t randomKey[64];
							XYO::System::Buffer rsaKey;
							rsaKey.setSize(rsaSize);

							XYO::Cryptography::SHA512 randomSalt;
							randomSalt.processInit();
							randomSalt.processU8(publicKey, publicKeySize);
							randomSalt.processU8(data, dataSize);
							UConvert::u64ToU8(DateTime::timestampInMilliseconds(), randomKey);
							randomSalt.processU8(randomKey, 8);
							randomSalt.processDone();
							randomSalt.toU8(randomKey);

							if (RSA_public_encrypt(
							        64,
							        randomKey,
							        rsaKey.buffer,
							        rsa, RSA_PKCS1_PADDING) == rsaKey.size) {

								rsaKey.length = rsaKey.size;
								RSA_free((RSA *)rsa);

								XYO::System::Buffer outputData;
								XYO::Cryptography::Crypt::encrypt(randomKey, 64, data, dataSize, outputData);
								output.setSize(rsaKey.length + outputData.length);
								output.length = rsaKey.length + outputData.length;
								memcpy(output.buffer, rsaKey.buffer, rsaKey.length);
								memcpy(&output.buffer[rsaKey.length], outputData.buffer, outputData.length);

								return true;
							};

							RSA_free((RSA *)rsa);
						};
					};

					return false;
				};

				bool publicDecrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output) {
					RSA *rsa = nullptr;
					BIO *bio = BIO_new_mem_buf(const_cast<uint8_t *>(publicKey), publicKeySize);
					if (bio != nullptr) {
						rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);
						BIO_free_all(bio);
						if (rsa != nullptr) {
							size_t rsaSize = RSA_size(rsa);
							if (rsaSize < 64) {
								RSA_free((RSA *)rsa);
								return false;
							};
							if (dataSize < rsaSize + 64 + 64 + 8) {
								return false;
							};

							XYO::System::Buffer randomKey;
							randomKey.setSize(rsaSize);

							int newLn = RSA_public_decrypt(
							    rsaSize,
							    data,
							    randomKey.buffer,
							    rsa, RSA_PKCS1_PADDING);

							RSA_free((RSA *)rsa);

							if (newLn != 64) {
								return false;
							};

							return XYO::Cryptography::Crypt::decrypt(randomKey.buffer, 64, &data[rsaSize], dataSize - rsaSize, output);
						};
					};

					return false;
				};

				bool privateEncryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut) {
					XYO::System::Buffer fileContents;
					if (Shell::fileGetContents(fileNameIn, fileContents)) {
						XYO::System::Buffer output;
						privateEncrypt(privateKey, privateKeySize, fileContents.buffer, fileContents.length, output);
						return Shell::filePutContents(fileNameOut, output);
					};
					return false;
				};

				bool privateDecryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut) {
					XYO::System::Buffer fileContents;
					if (Shell::fileGetContents(fileNameIn, fileContents)) {
						XYO::System::Buffer output;
						if (privateDecrypt(privateKey, privateKeySize, fileContents.buffer, fileContents.length, output)) {
							return Shell::filePutContents(fileNameOut, output);
						};
					};
					return false;
				};

				bool publicEncryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut) {
					XYO::System::Buffer fileContents;
					if (Shell::fileGetContents(fileNameIn, fileContents)) {
						XYO::System::Buffer output;
						publicEncrypt(publicKey, publicKeySize, fileContents.buffer, fileContents.length, output);
						return Shell::filePutContents(fileNameOut, output);
					};
					return false;
				};

				bool publicDecryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut) {
					XYO::System::Buffer fileContents;
					if (Shell::fileGetContents(fileNameIn, fileContents)) {
						XYO::System::Buffer output;
						if (publicDecrypt(publicKey, publicKeySize, fileContents.buffer, fileContents.length, output)) {
							return Shell::filePutContents(fileNameOut, output);
						};
					};
					return false;
				};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::OpenSSL::initExecutive(executive, extensionId);
};
#endif
