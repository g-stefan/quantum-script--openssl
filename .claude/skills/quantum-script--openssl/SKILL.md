---
name: quantum-script--openssl
description: >-
  How to use the Quantum Script OpenSSL extension (quantum-script--openssl),
  loaded with Script.requireExtension("OpenSSL") (also loads Buffer, Crypt,
  Socket, File, URL, JSON): HTTPS.json / post / postRequest /
  downloadFile(url, fileName, headers, mode, extra, content) (https://
  twins of HTTP, HTTP/1.0 Connection: close, status ignored, no redirects /
  timeouts, null / false when not connected or not https, Content-Length any
  case, default file name loads Shell); OpenSSL.sslConnect("host:port") TLS
  socket object (read, readLn, write, writeLn, readToBuffer,
  writeFromBuffer, close; undefined on failure; SNI sent); NO certificate
  verification (SSL_VERIFY_NONE); RSA hybrid encryption added to Crypt:
  Crypt.publicEncrypt / privateDecrypt / privateEncrypt / publicDecrypt and
  the ...File variants with PEM keys (BEGIN PUBLIC KEY only, unencrypted
  BEGIN PRIVATE KEY / BEGIN RSA PRIVATE KEY; encrypted keys rejected),
  Buffer or undefined, output rsaSize + 136 + 64 * (floor(n / 64) + 1),
  privateEncrypt is not a signature; OpenSSL.sha256 / sha512(buffer, start,
  length, out); raw RSA PKCS #1 v1.5 (rsaPublicKey, rsaPrivateKey, rsaSize,
  rsaPublicEncrypt, rsaPrivateDecrypt, rsaPrivateEncrypt, rsaPublicDecrypt;
  OpenSSL 3.2+ implicit rejection); low level ctxNew / bioConnect / bioRead
  / bioReadLn / bioReadToBuffer / bioClose; "invalid parameter" for wrong
  handles; hosts (quantum-script dynamic, magnet; not fabricare, not static
  quantum-script); C++ registerInternalExtension plus Buffer, Crypt, File,
  Socket, URL, JSON (Shell + ShellFind), and the C++ publicEncrypt /
  privateDecrypt / ...File functions. Use when writing or reviewing Quantum
  Script or magnet code that makes HTTPS requests, opens TLS connections,
  encrypts with RSA keys or calls Crypt.public* / Crypt.private*, C++ code
  that includes <XYO/QuantumScript.Extension/OpenSSL.hpp>, a fabricare.json
  depending on "quantum-script--openssl", or when working inside the
  quantum-script--openssl repository.
---

# quantum-script--openssl

TLS and RSA extension of Quantum Script (see the `quantum-script` skill for
the language and its differences from JavaScript, and the
`quantum-script--http` skill: `HTTPS` follows the same rules as `HTTP`).
Purpose: **HTTPS requests, TLS connections and RSA encryption from
scripts**. Native code: `source/XYO/QuantumScript.Extension/OpenSSL/Library.cpp`;
`HTTPS` and `sslConnect`: `OpenSSL/Library.js`. When in doubt read them.

Full documentation: `docs/` in the quantum-script--openssl repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--openssl\docs` on this machine):
README (purpose, warnings), getting-started (build, tests, hosts, C++
registration), **script-api** (every function, keys, encrypted format,
HTTPS request format, security), **recipes** (create / convert keys with
`openssl`, encrypt a message or file, Base64 storage, HTTPS JSON, auth
headers, verified download, line protocol over TLS, full response with
status, local TLS server), cpp-api (C++ functions, data format, maintainer
notes), reference (tables, results by situation). Read the matching page
when you need more than this summary.

## Script API

```javascript
Script.requireExtension("OpenSSL");          // also Buffer, Crypt, Socket, File, URL, JSON

// HTTPS: same as HTTP.* (quantum-script--http), for https:// URLs
var v = HTTPS.json("https://api.example.com/x", {a: 1});    // POST JSON -> value / undefined (not JSON) / null (no connection)
var t = HTTPS.post(url, "text", "Content-Type: text/plain", "POST");   // String / null
var ok = HTTPS.downloadFile(url, "file.zip");               // GET -> true / false (true also for 404 pages)
HTTPS.downloadFile(url, "f", headers, "POST", null, body);  // 5th argument "extra" is ignored

// TLS socket
var s = OpenSSL.sslConnect("host:443");      // undefined on failure; port required
s.writeLn("..."); var line = s.readLn(1024); // line with "\r\n", undefined when closed
var buffer = new Buffer(65536);
while (s.readToBuffer(buffer, Infinity) > 0) { /* buffer.toString() */ };
s.close();

// RSA hybrid encryption, PEM keys as String or Buffer
var enc = Crypt.publicEncrypt(publicPem, "data");       // Buffer / undefined
var dec = Crypt.privateDecrypt(privatePem, enc);        // Buffer / undefined (wrong key, damaged)
Crypt.publicEncryptFile(publicPem, "in", "out.enc");    // true / false; nothing written on failure
Crypt.privateDecryptFile(privatePem, "out.enc", "in");

// hashes and raw RSA
OpenSSL.sha256(buf, 0, buf.length, out);                // true / false; out resized to 32
var rsa = OpenSSL.rsaPublicKey(Buffer.fromString(publicPem));   // Resource / null
OpenSSL.rsaPublicEncrypt(rsa, Buffer.fromString("x"), outBuffer);  // true / false, PKCS #1 v1.5
```

## Hard rules

1. **No certificate verification** (`SSL_VERIFY_NONE`). Connections are
   encrypted, not authenticated: verify downloads with a known hash
   (`SHA256.fileHash`), do not trust responses on hostile networks, do not
   send secrets there. Do not claim HTTPS here protects against a man in the
   middle.
2. **`HTTPS` = `HTTP` rules**: HTTP/1.0, `Connection: close`, status code
   ignored (a `404` body is returned / saved, `downloadFile` gives `true`),
   no redirects, no timeouts, blocking. `headers` is one String joined with
   `"\r\n"`, never ending with `"\r\n"`. `post` data must be a String.
   `json` always sends a body. Call as methods (`HTTPS.json(...)`).
3. **Results, not exceptions**: `HTTPS.json` value / `undefined` / `null`;
   `post`, `postRequest` String / `null`; `downloadFile` `true` / `false`;
   `sslConnect` object / `undefined`; `Crypt.*` Buffer / `undefined`;
   `Crypt.*File` `true` / `false` (`undefined` if the key is not String /
   Buffer). Test with `Script.isNull` / `Script.isUndefined`, not `if (r)`.
4. **Keys**: public `-----BEGIN PUBLIC KEY-----` only (PKCS #1
   `BEGIN RSA PUBLIC KEY` gives `null` / `undefined`: convert with
   `openssl rsa -RSAPublicKey_in -pubout`); private `BEGIN PRIVATE KEY` or
   `BEGIN RSA PRIVATE KEY`, **unencrypted** (encrypted keys are rejected,
   no prompt). At least 512 bits for `Crypt.*`; use 2048+. Keys are created
   with the `openssl` tool, not by the extension.
5. **`privateEncrypt` / `publicDecrypt` is not a signature** and not
   confidential: anyone with the public key decrypts it and can forge data
   under the same header. Use `publicEncrypt` / `privateDecrypt` for secrecy
   and an external signature (`openssl dgst -sign`) for authenticity.
6. Encrypted size: `rsaSize + 136 + 64 * (floor(n / 64) + 1)` (2048 bit key,
   5 bytes → 456). Binary: store with `Shell.filePutContentsBuffer`,
   `Base64.encode(buffer)` or `toHex()`. Every call gives different bytes.
7. **Handles are typed**: passing `undefined`, a number, a key where a
   connection is expected, or a handle closed with `.close()` throws
   `invalid parameter`. Non-Buffer buffer arguments throw `invalid parameter`;
   empty raw RSA input throws `invalid length`.
8. **Raw RSA** is PKCS #1 v1.5: input at most `rsaSize - 11` bytes. With
   OpenSSL 3.2+ (Linux) `rsaPrivateDecrypt` of invalid data returns `true`
   with random bytes (implicit rejection); Windows (OpenSSL 1.1.1) returns
   `false`. Prefer `Crypt.*`, which authenticates the data.
9. `sslConnect` / `bioConnect` need `"host:port"` (`"[::1]:443"` for IPv6);
   `HTTPS.*` add `:443`. SNI is sent for host names, not IP addresses.
   `bioRead(bio, size)` without `size` returns `undefined`; `read` returns
   what is available (may be less than `size`); `""` at the end.
10. `HTTPS.downloadFile` without a file name takes the last URL path part and
    loads `Shell` itself; a path ending in `/` gives `false`. Download to
    `name.part`, check, then `Shell.rename`.
11. **Hosts**: dynamic `quantum-script` (with the `Buffer`, `Crypt`,
    `Socket`, `File`, `URL`, `JSON` DLLs and OpenSSL DLLs) and magnet
    (internal). **Not** fabricare build scripts, not the static
    `quantum-script` (`Unable to open "OpenSSL"`). The installed interpreter
    loads the SDK DLL, not `output/bin`: test a build with `fabricare test`.
12. Quantum Script syntax: `typeof(x)`, `var` only, blocks end with `};`,
    `substring(start, length)`, `throw(new Error("..."))`, `&&` / `||` give
    booleans.

## Patterns

```javascript
// encrypt for the key owner
var enc = Crypt.publicEncrypt(Shell.fileGetContents("public.pem"), secret);
if (Script.isUndefined(enc)) { throw(new Error("bad public key")); };
Shell.filePutContentsBuffer("secret.bin", enc);

var dec = Crypt.privateDecrypt(Shell.fileGetContents("private.pem"), Shell.fileGetContentsBuffer("secret.bin"));
if (Script.isUndefined(dec)) { throw(new Error("wrong key or damaged data")); };

// verified HTTPS download
Script.requireExtension("SHA256");
if (!HTTPS.downloadFile(url, "app.zip.part")) { throw(new Error("cannot connect")); };
if (SHA256.fileHash("app.zip.part") != expectedSha256) { Shell.removeFile("app.zip.part"); throw(new Error("bad download")); };
Shell.removeFile("app.zip"); Shell.rename("app.zip.part", "app.zip");

// status code: read the raw response
var s = OpenSSL.sslConnect("www.example.com:443");
s.writeLn("GET / HTTP/1.0"); s.writeLn("Host: www.example.com"); s.writeLn("");
var status = s.readLn(1024);           // "HTTP/1.1 200 OK\r\n"
```

## C++

```cpp
#include <XYO/QuantumScript.Extension/OpenSSL.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                     // host init callback
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::Crypt::registerInternalExtension(executive);
	Extension::File::registerInternalExtension(executive);
	Extension::Socket::registerInternalExtension(executive);
	Extension::URL::registerInternalExtension(executive);
	Extension::JSON::registerInternalExtension(executive);
	Extension::Shell::registerInternalExtension(executive);      // optional (downloadFile default name)
	Extension::ShellFind::registerInternalExtension(executive);  // required by Shell
	Extension::OpenSSL::registerInternalExtension(executive);    // scripts still requireExtension("OpenSSL")
};

// RSA hybrid without a script engine; keys = PEM bytes; false on any error
XYO::System::Buffer out;
Extension::OpenSSL::publicEncrypt(key.buffer, key.length, data, dataSize, out);
Extension::OpenSSL::privateDecryptFile(key.buffer, key.length, "in.enc", "out.txt");
```

- `fabricare.json` dependency `"quantum-script--openssl"` brings
  `quantum-script`, `--console`, `--buffer`, `--crypt`, `xyo-cryptography`
  and OpenSSL (Windows: vendor-openssl 1.1.1; Linux: system OpenSSL 3); also
  list `--file`, `--socket`, `--url`, `--json` (`--shell`, `--shellfind`).
  `dll-or-lib`: static library on static platforms, no
  `quantumScriptExtension` entry point there.

## Working in this repository

- Build: `fabricare make`, `fabricare test`, `fabricare install`,
  `fabricare clean` (see the `fabricare` skill). `test.01` (offline: SHA,
  RSA with `test/*.pem` throw away keys, Crypt, argument checks) and
  `test.02` (network: google.ro downloads, SNI with example.com /
  cloudflare) are C++ hosts in `test/` running `test/test.0N.js` against the
  library in `output/`. Add checks with `check(name, value, expected)`.
- Check Linux too (WSL `ubuntu-26.04`, OpenSSL 3) when touching native code.
- `fabricare/make.prepare.js` regenerates `Library.Source.cpp` from
  `Library.js` and `License.*.Source.cpp` from `License/*.txt`. Edit the
  sources, never the generated files (but commit them, they are tracked).
- Keep `HTTPS` (`OpenSSL/Library.js`) and `HTTP` (`quantum-script--http`)
  in step; the only intended differences are the transport, the default
  port (443) and the unused `extra` argument of `HTTPS.downloadFile`.
- Behavior changes: update `README.md`, `docs/`, and this skill.
- Code style: tabs, `.clang-format`, CRLF, `};` after blocks, camelCase.
  SPDX: MIT for `source/` and `docs/`, Unlicense for `test/`, `fabricare/`,
  `.claude/` (see `.reuse/dep5`; third-party license texts have their own
  entries).
