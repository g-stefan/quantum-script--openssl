//
// Quantum Script Extension OpenSSL
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include "xyo.hpp"

#ifdef XYO_OS_WINDOWS

#include <windows.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#include "quantum-script-extension-openssl-thread.hpp"


namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace OpenSSL {

				class Lock_ {
					public:
						bool initOk;
						static HANDLE *threadLock;

						static void lockingCallback(int mode, int type, const char *file, int line);

						Lock_();
						~Lock_();
				};

				HANDLE *Lock_::threadLock;

				Lock_::Lock_() {
					int i;

					initOk = false;

					if(SSL_library_init() < 0) {
						return;
					};

					ERR_load_BIO_strings();
					ERR_load_crypto_strings();
					SSL_load_error_strings();


					threadLock = (HANDLE *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
					for (i = 0; i < CRYPTO_num_locks(); i++) {
						threadLock[i] = CreateMutex(NULL, FALSE, NULL);
					};
					CRYPTO_set_locking_callback(lockingCallback);

					initOk = true;
				};

				Lock_::~Lock_() {
					if(initOk) {
						int i;
						CRYPTO_set_locking_callback(NULL);
						for (i = 0; i < CRYPTO_num_locks(); i++) {
							CloseHandle(threadLock[i]);
						};
						OPENSSL_free(threadLock);
					};
				};

				void Lock_::lockingCallback(int mode, int type, const char *file, int line) {
					if (mode & CRYPTO_LOCK) {
						WaitForSingleObject(threadLock[type], INFINITE);
					} else {
						ReleaseMutex(threadLock[type]);
					};
				};


				bool threadLockInit() {
					return (XYO::TSingletonProcess<Lock_>::getValue())->initOk;
				};

			};
		};
	};
};

#endif
