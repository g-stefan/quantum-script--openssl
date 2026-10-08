# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. Its dependencies must be
installed to the SDK first: `quantum-script` (and everything below it),
`quantum-script--console`, `quantum-script--buffer`, `quantum-script--crypt`,
`xyo-cryptography` and, on Windows, `vendor-openssl` (with `vendor-bzip2`,
`vendor-libxml2`, `vendor-libxslt`). On Linux the system OpenSSL
development package is used (`libssl-dev`). From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 and test/test.02
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

Before every `make`, `fabricare/make.prepare.js` turns
`OpenSSL/Library.js` into `Library.Source.cpp` and the texts in
`OpenSSL/License/` into `License.*.Source.cpp` (`file-to-cs`), so the script
is compiled into the library: edit `Library.js`, then build.

The library project is `quantum-script--openssl` with `"make": "dll-or-lib"`:

| Platform | Result | Use it when |
|----------|--------|-------------|
| dynamic (`~/.fabricare/win64-msvc-2026`, `ubuntu-*`) | `quantum-script--openssl.dll` / `libquantum-script--openssl.so` | scripts run by `quantum-script`, or a host using the engine DLL |
| static (`~/.fabricare/win64-msvc-2026.static`) | static library | self-contained hosts that register the extension as internal |

It is compiled with `OPENSSL_API_COMPAT=20000000` so the RSA and SHA
functions deprecated in OpenSSL 3 stay available.

### Tests

`fabricare test` builds two small C++ hosts that register `OpenSSL` and the
extensions it needs as internal extensions, link the library just built in
`output/`, and run a script:

| Test | Script | Needs |
|------|--------|-------|
| `test.01` | `test/test.01.js` | nothing: SHA, RSA with the test keys in `test/*.pem`, `Crypt`, argument checks, HTTPS argument handling |
| `test.02` | `test/test.02.js` | internet access: downloads from `www.google.ro`, Server Name Indication with `example.com` and `www.cloudflare.com` |

The PEM files in `test/` are throw away keys for the tests only.

Running a script with the installed `quantum-script` does **not** test your
build: the interpreter loads `quantum-script--openssl.dll` from the SDK
folder. Use `fabricare test`, or `fabricare install` first.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("OpenSSL");

// HTTPS
var result = HTTPS.json("https://api.example.com/v1/echo", {name: "test"});
if (Script.isNull(result)) {
	Console.writeLn("cannot connect");
} else if (Script.isUndefined(result)) {
	Console.writeLn("the response is not JSON");
} else {
	Console.writeLn(JSON.encode(result));
};

if (HTTPS.downloadFile("https://www.example.com/", "index.html")) {
	Console.writeLn("saved index.html");
};

// RSA hybrid encryption
Script.requireExtension("Shell");
var publicKey = Shell.fileGetContents("public.pem");
var privateKey = Shell.fileGetContents("private.pem");
var encrypted = Crypt.publicEncrypt(publicKey, "secret text");
Console.writeLn(Crypt.privateDecrypt(privateKey, encrypted).toString());
```

Run it with:

```bash
quantum-script openssl-example.js
```

`Script.requireExtension("OpenSSL")` looks for an external
`quantum-script--openssl` library first (as named, then next to the
interpreter, next to the script), then for an internal extension registered
by the host. Loading twice does nothing. A missing extension throws
`Unable to open "OpenSSL"`; a missing dependency throws the same error for
`Buffer`, `Crypt`, `Socket`, `File`, `URL` or `JSON`. On Windows the OpenSSL
DLLs (`libcrypto-1_1-x64.dll`, `libssl-1_1-x64.dll`) must be next to the
interpreter or on `PATH`; the SDK `bin` folder has them.

After it loads, the globals `OpenSSL`, `HTTPS`, `Crypt`, `Buffer`, `Socket`,
`File`, `URL` and `JSON` exist. `Shell` is loaded on demand by
`HTTPS.downloadFile` when no file name is given; require it yourself for
anything else.

### Which hosts have it

| Host | `OpenSSL` / `HTTPS` |
|------|---------------------|
| `quantum-script`, dynamic SDK | yes, `quantum-script--openssl.dll` / `.so` with its dependency DLLs |
| `quantum-script`, static SDK (`win64-msvc-2026.static`) | no: `Unable to open "OpenSSL"` |
| fabricare build scripts | no: fabricare does not embed `OpenSSL`; run a separate `quantum-script` process |
| magnet (`quantum-script--magnet`) | yes, internal (with `HTTP`, `File`, `Socket`, `URL`, `JSON`, `Shell`) |
| your own host | when it registers `OpenSSL` and its dependencies (section 3) |

## 3. Register it in a C++ host

A host that embeds Quantum Script makes `OpenSSL` available as an internal
extension by registering it, together with every extension it requires, in
the init callback (this is what `test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/Crypt.hpp>
#include <XYO/QuantumScript.Extension/File.hpp>
#include <XYO/QuantumScript.Extension/Socket.hpp>
#include <XYO/QuantumScript.Extension/URL.hpp>
#include <XYO/QuantumScript.Extension/JSON.hpp>
#include <XYO/QuantumScript.Extension/Shell.hpp>
#include <XYO/QuantumScript.Extension/ShellFind.hpp>
#include <XYO/QuantumScript.Extension/OpenSSL.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::Crypt::registerInternalExtension(executive);
	Extension::File::registerInternalExtension(executive);
	Extension::Socket::registerInternalExtension(executive);
	Extension::URL::registerInternalExtension(executive);
	Extension::JSON::registerInternalExtension(executive);
	Extension::Shell::registerInternalExtension(executive);      // optional: downloadFile without a file name
	Extension::ShellFind::registerInternalExtension(executive);  // required by Shell
	Extension::OpenSSL::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"OpenSSL\");"
		        "Console.writeLn(HTTPS.downloadFile(\"https://www.example.com/\", \"index.html\"));")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("OpenSSL")`. With the DLL build of the engine an
external `quantum-script--openssl.dll` found on the include path wins over
the internal one; use `Script.requireInternalExtension("OpenSSL")` to force
the internal one.

In the host's `fabricare.json` list the extensions you register that
`quantum-script--openssl` does not bring itself (it brings `quantum-script`,
`quantum-script--console`, `--buffer`, `--crypt`, `xyo-cryptography` and
OpenSSL):

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--openssl",
		"quantum-script--file",
		"quantum-script--socket",
		"quantum-script--url",
		"quantum-script--json",
		"quantum-script--shell",
		"quantum-script--shellfind"
	]
}
```

## 4. Static builds

There is no separate `quantum-script--openssl.static` project. On a static
fabricare platform the same project is built as a static library
(`dll-or-lib`), the `quantumScriptExtension` DLL entry point is left out (it
is compiled only with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY` and without
`XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_LIBRARY`), and the host registers the
extension and its dependencies with `registerInternalExtension` (section 3).
`quantum-script--magnet` is an example of a host that does this.

## 5. Threads

OpenSSL is initialized once per process, the first time an engine loads the
extension (`threadLockInit`, a process singleton). Each thread that runs
scripts has its own engine, so every thread loads the extension itself with
`Script.requireExtension("OpenSSL")` and gets its own `OpenSSL` and `HTTPS`
objects and its own 128 KB `HTTPS.buffer`. Keys, connections (`Resource`
values) belong to the thread that created them. A request blocks its thread
until the server closes the connection; run slow downloads in a `Thread` or
`Job` when the script must keep working meanwhile.
