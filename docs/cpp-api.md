# C++ API

Most programs use this extension from scripts. From C++ you register it in
a host, and you can call the RSA hybrid encryption directly, without a
script engine.

## Headers and namespace

```cpp
#include <XYO/QuantumScript.Extension/OpenSSL.hpp>   // Library.hpp

using namespace XYO::QuantumScript::Extension::OpenSSL;
```

`OpenSSL.hpp` includes `<XYO/QuantumScript.hpp>`. Exported symbols use
`XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_EXPORT`, which is `dllexport` while
building the DLL (`XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_INTERNAL`),
`dllimport` for its users, and empty for the static library
(`XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_LIBRARY`).

## Registering the extension

```cpp
void registerInternalExtension(Executive *executive);
void initExecutive(Executive *executive, void *extensionId);
```

- `registerInternalExtension(executive)` registers `"OpenSSL"` as an
  internal extension; call it in the host init callback, next to the
  extensions it needs (`Buffer`, `Crypt`, `File`, `Socket`, `URL`, `JSON`,
  optionally `Shell` + `ShellFind`). See
  [Getting started](getting-started.md#3-register-it-in-a-c-host).
- `initExecutive(executive, extensionId)` is what the engine runs when a
  script requires the extension: metadata (name, info with the OpenSSL
  license, version, public), OpenSSL initialization (`threadLockInit`,
  throws `Error("OpenSSL initialization")` on failure),
  `Script.requireExtension` of `Buffer` and `Crypt`, the native functions,
  then `librarySource` (the compiled `Library.js`).
- The DLL build exports `extern "C" void quantumScriptExtension(Executive *, void *)`,
  which calls `initExecutive`. It is left out of static builds.

### Dependencies at run time

| Required by | Extension |
|-------------|-----------|
| `initExecutive` | `Buffer`, `Crypt` |
| `Library.js` | `Socket`, `File`, `URL`, `JSON` |
| `HTTPS.downloadFile` without a file name | `Shell` (which needs `ShellFind` and `Buffer`) |

The `fabricare.json` of the library depends on `quantum-script`,
`quantum-script--console`, `quantum-script--buffer`, `quantum-script--crypt`
and `xyo-cryptography`; on Windows also `libbz2`, `libxml2`, `libxslt`,
`libcrypto`, `libssl` (vendor packages) and the system library `crypt32`; on
Linux the system `libcrypto` and `libssl`. A host lists `--file`,
`--socket`, `--url`, `--json` (and `--shell`, `--shellfind`) itself.

## RSA hybrid encryption from C++

The `Crypt.public*` / `Crypt.private*` script functions are thin wrappers
over these functions, which you can call directly:

```cpp
bool publicEncrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
bool privateDecrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
bool privateEncrypt(const uint8_t *privateKey, size_t privateKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);
bool publicDecrypt(const uint8_t *publicKey, size_t publicKeySize, const uint8_t *data, size_t dataSize, XYO::System::Buffer &output);

bool publicEncryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);
bool privateDecryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
bool privateEncryptFile(const uint8_t *privateKey, size_t privateKeySize, const char *fileNameIn, const char *fileNameOut);
bool publicDecryptFile(const uint8_t *publicKey, size_t publicKeySize, const char *fileNameIn, const char *fileNameOut);
```

- Keys are the bytes of a PEM file (`BEGIN PUBLIC KEY` for public keys,
  unencrypted `BEGIN PRIVATE KEY` / `BEGIN RSA PRIVATE KEY` for private
  keys), as described in [Script API](script-api.md#keys).
- All functions return `false` on any error; `output` is then unspecified.
  The file functions write `fileNameOut` only on success.
- The functions are independent of a script engine, but OpenSSL must be
  initialized; with OpenSSL 1.1 and 3 this happens automatically.

```cpp
#include <XYO/QuantumScript.Extension/OpenSSL.hpp>

using namespace XYO::System;
namespace OpenSSLX = XYO::QuantumScript::Extension::OpenSSL;

bool encryptForOwner(const char *publicKeyFile, const String &text, Buffer &output) {
	Buffer key;
	if (!Shell::fileGetContents(publicKeyFile, key)) {
		return false;
	};
	return OpenSSLX::publicEncrypt(key.buffer, key.length, (const uint8_t *)text.value(), text.length(), output);
};
```

### Encrypted data format

```
offset 0             RSA PKCS #1 v1.5 block, rsaSize bytes, holding a 64 byte session key
offset rsaSize       XYO::Cryptography::Crypt::encrypt(sessionKey, 64, data) output:
                     136 byte header (salt, key check, encrypted length), then the
                     data padded to a multiple of 64 bytes (one extra block when
                     the size is already a multiple)
```

The symmetric part is the format of `Crypt.encrypt` / `file-crypt`; it is
described in the `quantum-script--crypt` and `xyo-cryptography`
documentation.

Total: `rsaSize + 136 + 64 * (floor(n / 64) + 1)`. The session key is
`SHA512(key PEM, data, time in ms, 64 bytes of SystemRandom)`; decryption
does not depend on how it was made, so data written by older versions of the
extension (without `SystemRandom`) still decrypts.

`privateEncrypt` puts the session key under the private key: everyone with
the public key can recover it, so the result is neither confidential nor a
signature (see [Script API](script-api.md#crypt-rsa-hybrid-encryption)).

## How it works

- `ctxNew` creates an `SSL_CTX` with `SSLv23_client_method()` and
  `SSL_VERIFY_NONE`.
- `bioConnect` creates `BIO_new_ssl_connect(ctx)`, sets the SNI host name
  (`SSL_set_tlsext_host_name`, skipped for IPv4 / IPv6 literals), sets the
  connect address with `BIO_set_conn_hostname` and runs `SSL_do_handshake`.
- Handles are `VariableResource` values; every native function checks that
  its handle is a `VariableResource` with the expected delete function
  (`ctxDelete`, `bioDelete`, `rsaDelete`) and a non null resource, otherwise
  it throws `invalid parameter`.
- Keys are read with `PEM_read_bio_RSA_PUBKEY` / `PEM_read_bio_RSAPrivateKey`
  and a password callback that refuses, so OpenSSL never prompts on the
  console.
- `Thread-OS-Windows.cpp` / `Thread-OS-Linux.cpp`: `threadLockInit()`
  creates, once per process (`TSingletonProcess`), the OpenSSL library
  initialization and the locking callbacks needed by OpenSSL 1.0. With
  OpenSSL 1.1 and later these calls are no-ops kept for compatibility.

## Notes for maintainers

- Edit `OpenSSL/Library.js`, never `Library.Source.cpp`:
  `fabricare/make.prepare.js` regenerates it (and the license
  `.Source.cpp` files) before each build.
- `HTTPS` is a copy of `HTTP` from `quantum-script--http` over
  `OpenSSL.sslConnect`. Keep the two in step when changing behavior; the
  only intended differences are the transport, the default port (443) and
  the unused `extra` argument of `HTTPS.downloadFile`.
- The OpenSSL RSA and SHA functions used here are deprecated in OpenSSL 3;
  `OPENSSL_API_COMPAT=20000000` in `fabricare.json` keeps them. Moving to the
  `EVP_PKEY` API must keep the encrypted data format above.
- Tests: `test/test.01.cpp` and `test/test.02.cpp` are hosts that register
  the extensions as internal and run `test/test.01.js` (offline) and
  `test/test.02.js` (network). Add offline checks to `test.01.js` with the
  `check(name, value, expected)` pattern.
- Build and test on Windows (`win64-msvc-2026`) and on Linux (WSL) when
  touching the native code: Windows links OpenSSL 1.1.1, Linux OpenSSL 3,
  and their behavior differs (for example RSA implicit rejection).
- Behavior changes: update `README.md`, `docs/`, and the skill in
  `.claude/skills/quantum-script--openssl/`.
- Code style: tabs, `.clang-format`, CRLF, statements and blocks end with
  `};`. SPDX headers: MIT in `source/` and `docs/`, Unlicense in `test/`,
  `fabricare/` and `.claude/` (see `.reuse/dep5`).
