# API reference

## Script

Available after `Script.requireExtension("OpenSSL")`, which also loads
`Buffer`, `Crypt`, `Socket`, `File`, `URL` and `JSON`.

### HTTPS

| Symbol | Returns | Notes |
|--------|---------|-------|
| `HTTPS.json(url, data, headers, mode)` | value / `undefined` / `null` | body `JSON.encode(data)`, default `POST`, `undefined` when the body is not JSON |
| `HTTPS.post(url, data, headers, mode)` | String / `null` | `data` a String, `text/plain` unless a `Content-Type:` header is given |
| `HTTPS.postRequest(url, data, headers, mode)` | String / `null` | one multipart field `request`, non standard layout |
| `HTTPS.downloadFile(url, fileName, headers, mode, extra, content)` | Boolean | default `GET`; `extra` ignored; default `fileName` from the URL path |
| `HTTPS.decodeHeaderLine(line)` | `[name, value]` / `false` | |
| `HTTPS.buffer` | Buffer | 128 KB, shared |

### TLS connection

| Symbol | Returns | Notes |
|--------|---------|-------|
| `OpenSSL.sslConnect(server)` | object / `undefined` | `server` = `"host:port"`; methods below |
| `.isValid()` | `true` | |
| `.read(size)` | String / `undefined` | up to `size` bytes |
| `.readLn(size)` | String / `undefined` | one line with `"\r\n"`; `undefined` when closed |
| `.write(str)`, `.writeLn(str)` | Number | bytes written |
| `.readToBuffer(buffer, size)` | Number | bytes read, `0` at the end; `size` may be `Infinity` |
| `.writeFromBuffer(buffer)` | Number | bytes written |
| `.close()` | `undefined` | TLS shutdown |

### Crypt (RSA hybrid)

| Symbol | Returns | Notes |
|--------|---------|-------|
| `Crypt.publicEncrypt(publicKey, data)` | Buffer / `undefined` | for the private key owner |
| `Crypt.privateDecrypt(privateKey, data)` | Buffer / `undefined` | `undefined`: wrong key, damaged data |
| `Crypt.privateEncrypt(privateKey, data)` | Buffer / `undefined` | not a signature |
| `Crypt.publicDecrypt(publicKey, data)` | Buffer / `undefined` | |
| `Crypt.publicEncryptFile(key, fileIn, fileOut)` | Boolean / `undefined` | `undefined` when `key` is not String / Buffer |
| `Crypt.privateDecryptFile(key, fileIn, fileOut)` | Boolean / `undefined` | |
| `Crypt.privateEncryptFile(key, fileIn, fileOut)` | Boolean / `undefined` | |
| `Crypt.publicDecryptFile(key, fileIn, fileOut)` | Boolean / `undefined` | |

Output size: `rsaSize + 136 + 64 * (floor(n / 64) + 1)` bytes.

### Hashes

| Symbol | Returns | Notes |
|--------|---------|-------|
| `OpenSSL.sha256Length`, `OpenSSL.sha512Length` | 32, 64 | |
| `OpenSSL.sha256(bufferIn, start, length, hashOut)` | Boolean | `false`: `length` 0 or range outside `bufferIn.length` |
| `OpenSSL.sha512(bufferIn, start, length, hashOut)` | Boolean | |

### Raw RSA (PKCS #1 v1.5)

| Symbol | Returns | Notes |
|--------|---------|-------|
| `OpenSSL.rsaPublicKey(buffer)` | Resource / `null` | `BEGIN PUBLIC KEY` only |
| `OpenSSL.rsaPrivateKey(buffer)` | Resource / `null` | unencrypted PKCS #8 or PKCS #1 |
| `OpenSSL.rsaSize(rsa)` | Number | bytes |
| `OpenSSL.rsaPublicEncrypt(rsa, buffer, bufferOut)` | Boolean | input at most `rsaSize - 11` bytes |
| `OpenSSL.rsaPrivateDecrypt(rsa, buffer, bufferOut)` | Boolean | OpenSSL 3.2+: random bytes instead of `false` for invalid data |
| `OpenSSL.rsaPrivateEncrypt(rsa, buffer, bufferOut)` | Boolean | |
| `OpenSSL.rsaPublicDecrypt(rsa, buffer, bufferOut)` | Boolean | |

### Low level TLS

| Symbol | Returns | Notes |
|--------|---------|-------|
| `OpenSSL.ctxNew()` | Resource / `undefined` | client context, no certificate verification |
| `OpenSSL.sslNew(ctx)` | Resource / `undefined` | |
| `OpenSSL.bioConnect(ctx, server)` | Resource / `undefined` | connect, SNI, handshake |
| `OpenSSL.bioClose(bio)` | `undefined` | TLS shutdown |
| `OpenSSL.bioRead(bio, size)` | String / `undefined` | `undefined` for an invalid `size` |
| `OpenSSL.bioReadLn(bio, size)` | String / `undefined` | |
| `OpenSSL.bioWrite(bio, str)`, `OpenSSL.bioWriteLn(bio, str)` | Number | |
| `OpenSSL.bioReadToBuffer(bio, buffer, size)` | Number | |
| `OpenSSL.bioWriteFromBuffer(bio, buffer)` | Number | |

### Results by situation

| Situation | `json`, `post`, `postRequest` | `downloadFile` | `sslConnect` | `Crypt.*` | `Crypt.*File` |
|-----------|------------------------------|----------------|--------------|-----------|---------------|
| success | value / String | `true` | object | Buffer | `true` |
| URL not `https://`, no `://`, empty host | `null` | `false` | | | |
| cannot connect, handshake failed | `null` | `false` | `undefined` | | |
| HTTP error status (`404`, `500`, `302`) | the error body | `true`, file saved | | | |
| key or data not String / Buffer | | | | `undefined` | `undefined` (key) |
| key unreadable, encrypted, too small | | | | `undefined` | `false` |
| wrong key, damaged data | | | | `undefined` | `false` |
| input file missing | | | | | `false` |

### Errors

| Message | Cause |
|---------|-------|
| `invalid parameter` | a `Buffer` argument that is not a `Buffer`; a handle of the wrong kind, `undefined`, or closed |
| `invalid length` | raw RSA with an empty input |
| `OpenSSL initialization` | OpenSSL could not be initialized |
| `Unable to open "OpenSSL"` | the extension or a dependency (named) was not found |

## Hosts

| Host | Available |
|------|-----------|
| `quantum-script`, dynamic SDK | yes (DLL / `.so`) |
| `quantum-script`, static SDK | no |
| fabricare build scripts | no |
| magnet | yes, internal |
| your host | after `registerInternalExtension` |

## C++

Namespace `XYO::QuantumScript::Extension::OpenSSL`, umbrella header
`<XYO/QuantumScript.Extension/OpenSSL.hpp>`.

### Library (`OpenSSL/Library.hpp`)

| Symbol | Notes |
|--------|-------|
| `void registerInternalExtension(Executive *executive)` | register `"OpenSSL"` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | extension init, run by the engine |
| `bool publicEncrypt(publicKey, publicKeySize, data, dataSize, Buffer &output)` | RSA hybrid, PEM key bytes |
| `bool privateDecrypt(privateKey, privateKeySize, data, dataSize, Buffer &output)` | |
| `bool privateEncrypt(privateKey, privateKeySize, data, dataSize, Buffer &output)` | |
| `bool publicDecrypt(publicKey, publicKeySize, data, dataSize, Buffer &output)` | |
| `bool publicEncryptFile(publicKey, publicKeySize, fileNameIn, fileNameOut)` | writes only on success |
| `bool privateDecryptFile(privateKey, privateKeySize, fileNameIn, fileNameOut)` | |
| `bool privateEncryptFile(privateKey, privateKeySize, fileNameIn, fileNameOut)` | |
| `bool publicDecryptFile(publicKey, publicKeySize, fileNameIn, fileNameOut)` | |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | DLL entry point (not in static builds) |

### Thread (`OpenSSL/Thread.hpp`)

| Symbol | Notes |
|--------|-------|
| `bool threadLockInit()` | one time OpenSSL initialization per process; `true` when ready |

### Metadata

| Symbol | Notes |
|--------|-------|
| `Copyright::copyright()`, `Copyright::publisher()`, ... | `OpenSSL/Copyright.hpp` |
| `License::license()` | MIT text plus the bzip2, libxml2, libxslt and OpenSSL licenses |
| `License::shortLicense()` | |
| `Version::version()`, `Version::versionWithBuild()`, ... | `OpenSSL/Version.hpp`, from `version.json` |

### Build configuration

| Define | Effect |
|--------|--------|
| `OPENSSL_API_COMPAT=20000000` | set by `fabricare.json`, keeps the deprecated RSA / SHA functions on OpenSSL 3 |
| `XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_INTERNAL` | building the DLL (exports) |
| `XYO_QUANTUMSCRIPT_EXTENSION_OPENSSL_LIBRARY` | static library: empty export macro, no DLL entry point |
