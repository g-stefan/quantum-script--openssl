//
// Quantum Script Extension OpenSSL
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
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

					static const char *version_ = "2.7.0";
					static const char *build_ = "23";
					static const char *versionWithBuild_ = "2.7.0.23";
					static const char *datetime_ = "2022-01-01 21:19:45";

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



