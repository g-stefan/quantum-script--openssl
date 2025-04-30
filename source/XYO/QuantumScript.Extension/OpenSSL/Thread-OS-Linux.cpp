// Quantum Script Extension OpenSSL
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/OpenSSL/Thread.hpp>

#ifdef XYO_PLATFORM_OS_LINUX

#	include <pthread.h>
#	include <unistd.h>
#	include <stdio.h>
#	include <openssl/ssl.h>
#	include <openssl/err.h>
#	include <openssl/crypto.h>

namespace XYO::QuantumScript::Extension::OpenSSL {
	using namespace XYO::ManagedMemory;

	class Lock_ {
		public:
			bool initOk;
			static pthread_mutex_t *threadLock;

			static void lockingCallback(int mode, int type, const char *file, int line);

			Lock_();
			~Lock_();
	};

	pthread_mutex_t *Lock_::threadLock;

	Lock_::Lock_() {
		int i;

		initOk = false;

		if (SSL_library_init() < 0) {
			return;
		};

		ERR_load_BIO_strings();
		ERR_load_crypto_strings();
		SSL_load_error_strings();

		threadLock = (pthread_mutex_t *)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
		for (i = 0; i < CRYPTO_num_locks(); i++) {
			pthread_mutex_init(&threadLock[i], NULL);
		};
		CRYPTO_set_locking_callback(lockingCallback);

		initOk = true;
	};

	Lock_::~Lock_() {
		if (initOk) {
			int i;
			CRYPTO_set_locking_callback(NULL);
			for (i = 0; i < CRYPTO_num_locks(); i++) {
				pthread_mutex_destroy(&threadLock[i]);
			};
			OPENSSL_free(threadLock);
		};
	};

	void Lock_::lockingCallback(int mode, int type, const char *file, int line) {
		if (mode & CRYPTO_LOCK) {
			pthread_mutex_lock(&threadLock[type]);
		} else {
			pthread_mutex_unlock(&threadLock[type]);
		};
	};

	bool threadLockInit() {
		return (TSingletonProcess<Lock_>::getValue())->initOk;
	};

};

#endif
