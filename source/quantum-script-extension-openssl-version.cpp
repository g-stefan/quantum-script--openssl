//
// Quantum Script Extension OpenSSL
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include "quantum-script-extension-openssl-version.hpp"

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace OpenSSL {
				namespace Version {

					static const char *version_ = "1.5.0";
					static const char *build_ = "7";
					static const char *versionWithBuild_ = "1.5.0.7";
					static const char *datetime_ = "2021-07-06 15:11:49";

					const char *version() {
						return version_;
					};
					const char *build() {
						return build_;
					};
					const char *versionWithBuild() {
						return versionWithBuild_;
					};
					const char *datetime() {
						return datetime_;
					};

				};
			};
		};
	};
};



