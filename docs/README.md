# Quantum Script Extension OpenSSL — Documentation

`quantum-script--openssl` is the **TLS and RSA extension of Quantum Script**.
Loaded with `Script.requireExtension("OpenSSL")`, it gives scripts:

- **HTTPS requests** with one call: `HTTPS.json`, `HTTPS.post`,
  `HTTPS.postRequest`, `HTTPS.downloadFile` — the `https://` twins of the
  `HTTP` extension.
- **TLS client connections** to any server: `OpenSSL.sslConnect("host:port")`
  returns a socket like object with `read`, `readLn`, `write`, `writeLn`,
  `readToBuffer`, `writeFromBuffer`, `close`.
- **RSA hybrid encryption** of strings, buffers and files with PEM keys:
  `Crypt.publicEncrypt` / `Crypt.privateDecrypt` (and the reverse pair
  `Crypt.privateEncrypt` / `Crypt.publicDecrypt`), plus the `...File`
  variants. These are added to the `Crypt` object of `quantum-script--crypt`.
- **Raw RSA** (PKCS #1 v1.5) on buffers: `OpenSSL.rsaPublicKey`,
  `rsaPrivateKey`, `rsaPublicEncrypt`, `rsaPrivateDecrypt`, ...
- **SHA-256 / SHA-512** of a part of a buffer: `OpenSSL.sha256`,
  `OpenSSL.sha512`.

The native part (`OpenSSL/Library.cpp`) wraps OpenSSL (`libssl`,
`libcrypto`); the `HTTPS` object and `OpenSSL.sslConnect` are written in
Quantum Script (`OpenSSL/Library.js`, compiled into the library).

```
scripts: quantum-script .js, magnet, hosts that register the extension
quantum-script--openssl   <-- this extension: OpenSSL.*, HTTPS.*, Crypt.public* / private*
quantum-script--crypt     (Crypt.encrypt / decrypt: symmetric part of the RSA hybrid format)
quantum-script--buffer    (Buffer)
quantum-script--socket, --url, --json, --file   (loaded by Library.js for HTTPS)
quantum-script            (Executive, script engine)
xyo-cryptography          (SHA512, Crypt, SystemRandom)
OpenSSL: libssl, libcrypto (Windows: vendor-openssl 1.1.1, Linux: system OpenSSL 3)
xyo-system, xyo-encoding, ... , xyo-platform
```

## Why it exists

| Need | What it gives |
|------|---------------|
| Call a JSON API over `https://` from a script | `HTTPS.json(url, data)`, one call, decoded result |
| Download a release archive or data file over `https://` | `HTTPS.downloadFile(url, fileName)`, streamed through a 128 KB buffer |
| Speak another protocol over TLS (SMTP after the handshake, custom servers) | `OpenSSL.sslConnect("host:port")` and line / buffer I/O |
| Encrypt data or files so only the private key owner can read them | `Crypt.publicEncrypt(publicKeyPem, data)` / `Crypt.privateDecrypt(privateKeyPem, data)` |
| Hash part of a buffer with OpenSSL | `OpenSSL.sha256(buffer, start, length, out)` |

## Read this first

- **Server certificates are not verified.** The TLS context uses
  `SSL_VERIFY_NONE`: the connection is encrypted but the server is not
  authenticated, so a man in the middle can impersonate it. Do not send
  secrets to, or trust downloads from, a network you do not control without
  an extra check (compare a known `SHA256` / `SHA512` of the download, sign
  your payloads). See [Script API](script-api.md#security).
- **HTTPS is HTTP/1.0 with `Connection: close`**: no status code, no
  redirects, no chunked bodies, no timeouts. Exactly the same rules as the
  `HTTP` extension.
- **`Crypt.privateEncrypt` is not a signature.** Anyone with the public key
  can decrypt it and, with the recovered session key, produce new data that
  `Crypt.publicDecrypt` accepts. Use `publicEncrypt` / `privateDecrypt` for
  confidentiality.
- **Keys are PEM text, unencrypted.** Public keys must be
  `-----BEGIN PUBLIC KEY-----` (X.509 SubjectPublicKeyInfo); private keys
  `-----BEGIN PRIVATE KEY-----` or `-----BEGIN RSA PRIVATE KEY-----`.
  Password protected keys are rejected (no console prompt).

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("OpenSSL");` | also loads `Buffer`, `Crypt`, `Socket`, `File`, `URL`, `JSON` |
| JSON over HTTPS | `HTTPS.json(url, data)` | `POST`; `undefined` when the body is not JSON, `null` when not connected |
| Text over HTTPS | `HTTPS.post(url, text, headers, mode)` | `Content-Type: text/plain` unless given |
| Download | `HTTPS.downloadFile(url, "file.zip")` | `true` also for `404` pages: check the content |
| TLS socket | `var s = OpenSSL.sslConnect("host:443")` | `undefined` when the connection or handshake fails |
| Encrypt for a key owner | `Crypt.publicEncrypt(publicPem, data)` | `Buffer`, `rsaSize + 136 + 64 * (floor(n / 64) + 1)` bytes |
| Decrypt | `Crypt.privateDecrypt(privatePem, data)` | `Buffer`, or `undefined` (wrong key, damaged data) |
| Files | `Crypt.publicEncryptFile(publicPem, in, out)` | `true` / `false` |
| Hash | `OpenSSL.sha256(buffer, 0, buffer.length, out)` | `true` / `false`, `out` resized to 32 bytes |
| Wrong handle passed | any `OpenSSL.bio*` / `rsa*` | throws `invalid parameter` |

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build, test, load from a script, which hosts have it, register in a C++ host, static builds, threads |
| [Script API](script-api.md) | Every function: arguments, results, errors, the HTTPS request format, security notes |
| [Recipes](recipes.md) | Keys with the `openssl` tool, encrypt a file for someone, HTTPS JSON calls, verified downloads, a TLS line protocol, hashing files |
| [C++ API](cpp-api.md) | `registerInternalExtension`, the `privateEncrypt` / `publicDecrypt` / ... C++ functions, the encrypted data format, notes for maintainers |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding)
is documented in the `quantum-script` repository, `docs/`; `Crypt`,
`Buffer`, `HTTP`, `Socket`, `URL`, `JSON` in their own repositories.

## Source map

```
source/XYO/QuantumScript.Extension/OpenSSL.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/OpenSSL.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/OpenSSL/
    Dependency.hpp                                        <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                    native functions, initExecutive,
                                                          registerInternalExtension, C++ RSA hybrid API,
                                                          quantumScriptExtension DLL entry point
    Library.js                                            OpenSSL.sslConnect and the HTTPS object
    Library.Source.cpp                                    generated from Library.js (librarySource)
    Thread[.hpp/-OS-Windows.cpp/-OS-Linux.cpp]            OpenSSL one time initialization (threadLockInit)
    License/*.txt, License.*.Source.cpp                   bzip2, libxml2, libxslt, OpenSSL license texts
    Copyright / License / Version                         library metadata
fabricare/make.prepare.js                                 file-to-cs: Library.js and License/*.txt to .Source.cpp
test/test.01.*                                            offline tests: SHA, RSA, Crypt, argument checks
test/test.02.*                                            network tests: HTTPS downloads, SNI
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--openssl/`](../.claude/skills/quantum-script--openssl/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use
`OpenSSL` / `HTTPS` (Quantum Script tools, magnet scripts, hosts).
