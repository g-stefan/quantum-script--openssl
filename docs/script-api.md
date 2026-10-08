# Script API

```javascript
Script.requireExtension("OpenSSL");   // also Buffer, Crypt, Socket, File, URL, JSON

// HTTPS (Library.js)
HTTPS.buffer;                                                     // Buffer, 128 * 1024, shared by all calls
HTTPS.decodeHeaderLine(headerLine);                               // [name, value] or false
HTTPS.json(url, data, headers, mode);                             // decoded JSON, undefined, or null
HTTPS.post(url, data, headers, mode);                             // response body String, or null
HTTPS.postRequest(url, data, headers, mode);                      // response body String, or null
HTTPS.downloadFile(url, fileName, headers, mode, extra, content); // true / false

// TLS connection (Library.js)
OpenSSL.sslConnect(server);          // object, or undefined

// RSA hybrid encryption (added to Crypt)
Crypt.publicEncrypt(publicKey, data);               // Buffer, or undefined
Crypt.privateDecrypt(privateKey, data);             // Buffer, or undefined
Crypt.privateEncrypt(privateKey, data);             // Buffer, or undefined
Crypt.publicDecrypt(publicKey, data);               // Buffer, or undefined
Crypt.publicEncryptFile(publicKey, fileIn, fileOut);   // true / false, or undefined
Crypt.privateDecryptFile(privateKey, fileIn, fileOut); // true / false, or undefined
Crypt.privateEncryptFile(privateKey, fileIn, fileOut); // true / false, or undefined
Crypt.publicDecryptFile(publicKey, fileIn, fileOut);   // true / false, or undefined

// hashes
OpenSSL.sha256Length;                           // 32
OpenSSL.sha512Length;                           // 64
OpenSSL.sha256(bufferIn, start, length, hashOut);  // true / false
OpenSSL.sha512(bufferIn, start, length, hashOut);  // true / false

// raw RSA, PKCS #1 v1.5
OpenSSL.rsaPublicKey(buffer);                   // Resource, or null
OpenSSL.rsaPrivateKey(buffer);                  // Resource, or null
OpenSSL.rsaSize(rsa);                           // Number, bytes
OpenSSL.rsaPublicEncrypt(rsa, buffer, bufferOut);   // true / false
OpenSSL.rsaPrivateDecrypt(rsa, buffer, bufferOut);  // true / false
OpenSSL.rsaPrivateEncrypt(rsa, buffer, bufferOut);  // true / false
OpenSSL.rsaPublicDecrypt(rsa, buffer, bufferOut);   // true / false

// low level TLS
OpenSSL.ctxNew();                               // Resource (SSL_CTX), or undefined
OpenSSL.sslNew(ctx);                            // Resource (SSL), or undefined
OpenSSL.bioConnect(ctx, server);                // Resource (BIO), or undefined
OpenSSL.bioClose(bio);
OpenSSL.bioRead(bio, size);                     // String, or undefined
OpenSSL.bioReadLn(bio, size);                   // String, or undefined
OpenSSL.bioWrite(bio, str);                     // Number
OpenSSL.bioWriteLn(bio, str);                   // Number
OpenSSL.bioReadToBuffer(bio, buffer, size);     // Number
OpenSSL.bioWriteFromBuffer(bio, buffer);        // Number
```

`OpenSSL`, `HTTPS` and `Crypt` are plain objects (`typeof(...) == "Object"`).
Keys and connections are `Resource` values (`typeof(x) == "Resource"`): the
native object is freed when the value is collected.

## Loading

```javascript
Script.requireExtension("OpenSSL");
```

`initExecutive` initializes OpenSSL (once per process; failure throws
`OpenSSL initialization`), runs `Script.requireExtension` for `Buffer` and
`Crypt`, defines the native functions, then runs `Library.js`, which requires
`Socket`, `File`, `URL` and `JSON`, defines `OpenSSL.sslConnect` and creates
`HTTPS` (only if it does not exist yet). The extension is public, versioned
and listed by `Script.getExtensionList()` as `OpenSSL`.

## Security

- **No certificate verification.** Every connection (`HTTPS.*`,
  `sslConnect`, `bioConnect`) uses a context with `SSL_VERIFY_NONE`: the
  traffic is encrypted, but the server certificate, its chain and its host
  name are not checked. An attacker on the network path can answer instead
  of the real server. Treat responses as untrusted input; verify downloads
  with a known hash ([Recipes](recipes.md#verified-download)); do not send
  passwords or tokens over networks you do not control.
- **TLS version**: whatever the linked OpenSSL negotiates as a client
  (`SSLv23_client_method`): up to TLS 1.3 on OpenSSL 1.1.1 (Windows) and
  OpenSSL 3 (Linux).
- **Server Name Indication** is sent with the host name part of `server`
  (not for IP addresses), so servers that host several sites (CDNs) answer
  with the right certificate and do not drop the handshake.
- **RSA padding** is PKCS #1 v1.5 everywhere. With OpenSSL 3.2 and later,
  decrypting invalid data with the private key does not fail: it returns
  random bytes (implicit rejection, a defence against padding oracles).
  `Crypt.privateDecrypt` is not affected (its symmetric part is
  authenticated); the raw `OpenSSL.rsaPrivateDecrypt` result must be checked
  by your own protocol.
- **Session keys**: `Crypt.publicEncrypt` / `privateEncrypt` draw a fresh
  64 byte key for every call from the operating system random generator
  (mixed with the key, the data and the time), so the same input never gives
  the same output.

## Accepted arguments

| Argument | Accepted | Otherwise |
|----------|----------|-----------|
| `publicKey`, `privateKey` (`Crypt.*`) | `String` or `Buffer` holding PEM text | `undefined` returned |
| `data` (`Crypt.*`) | `String` or `Buffer` (its first `length` bytes) | `undefined` returned |
| `fileIn`, `fileOut` | converted with `toString()`, relative to the process current directory | |
| `buffer`, `bufferIn`, `bufferOut`, `hashOut` (`OpenSSL.*`) | `Buffer` | throws `invalid parameter` |
| `rsa` | a key from `rsaPublicKey` / `rsaPrivateKey` | throws `invalid parameter` |
| `ctx` | a value from `ctxNew` | throws `invalid parameter` |
| `bio` | a value from `bioConnect`, not closed with `.close()` | throws `invalid parameter` |
| `size`, `start`, `length` | numbers | see each function |

Passing a handle of the wrong kind (a key where a connection is expected,
`undefined`, a number) throws `invalid parameter` instead of crashing.

## Keys

Keys are PEM text, read with OpenSSL:

| PEM header | `rsaPublicKey` / `Crypt.public*` | `rsaPrivateKey` / `Crypt.private*` |
|------------|------------------------|-------------------------|
| `-----BEGIN PUBLIC KEY-----` (X.509, `openssl rsa -pubout`) | yes | no |
| `-----BEGIN RSA PUBLIC KEY-----` (PKCS #1) | **no**, `null` / `undefined` | no |
| `-----BEGIN PRIVATE KEY-----` (PKCS #8, `openssl genpkey`) | no | yes |
| `-----BEGIN RSA PRIVATE KEY-----` (PKCS #1, `openssl genrsa` 1.1, `-traditional`) | no | yes |
| `-----BEGIN ENCRYPTED PRIVATE KEY-----` or `Proc-Type: 4,ENCRYPTED` | no | **no**: rejected, no password prompt |

A key smaller than 512 bits (`rsaSize < 64`) is rejected by the `Crypt.*`
functions. Use 2048 bits or more. See
[Recipes](recipes.md#create-a-key-pair) to create and convert keys.

## Crypt: RSA hybrid encryption

The `Crypt.*` functions combine RSA with the symmetric `Crypt.encrypt` of
`quantum-script--crypt`:

1. a random 64 byte session key is made;
2. it is encrypted with RSA (PKCS #1 v1.5) using the given key;
3. the data is encrypted and authenticated with `Crypt.encrypt(sessionKey, data)`;
4. the result is the RSA block (`rsaSize` bytes) followed by the `Crypt` data.

```
+---------------------------+---------------------------------------------+
| RSA(session key)          | Crypt.encrypt(session key, data)            |
| rsaSize bytes (256: 2048) | 136 + 64 * (floor(n / 64) + 1) bytes        |
+---------------------------+---------------------------------------------+
```

For a 2048 bit key, 0 to 63 bytes of data give 456 bytes, 64 to 127 give 520,
and so on.

| Pair | Who can encrypt | Who can decrypt | Use it for |
|------|-----------------|-----------------|------------|
| `publicEncrypt` → `privateDecrypt` | anyone with the public key | only the private key owner | confidentiality: send data to the key owner |
| `privateEncrypt` → `publicDecrypt` | the private key owner | anyone with the public key | obfuscation only, **not** a signature (see below) |

`privateEncrypt` output is not authentic: whoever decrypts it with the public
key learns the session key and can encrypt other data under the same RSA
block, which `publicDecrypt` then accepts. Use a real signature (for example
`openssl dgst -sha256 -sign`) when the receiver must know who wrote the data.

### Crypt.publicEncrypt(publicKey, data) / Crypt.privateEncrypt(privateKey, data)

```javascript
var encrypted = Crypt.publicEncrypt(publicKeyPem, "message");   // Buffer
Shell.filePutContentsBuffer("message.bin", encrypted);
```

Returns a new `Buffer`, or `undefined` when an argument is not a `String` /
`Buffer`, the key cannot be read (wrong PEM type, encrypted key, garbage) or
is smaller than 512 bits. Empty data is valid.

### Crypt.privateDecrypt(privateKey, data) / Crypt.publicDecrypt(publicKey, data)

```javascript
var decrypted = Crypt.privateDecrypt(privateKeyPem, encrypted);
if (Script.isUndefined(decrypted)) {
	throw(new Error("wrong key or damaged data"));
};
var text = decrypted.toString();
```

Returns the original bytes as a new `Buffer`, or `undefined` when the
arguments are not accepted, the key cannot be read, the data is shorter
than `rsaSize + 136` bytes, the RSA block does not decrypt to 64 bytes
(other key), or the `Crypt` part fails its check (damaged or changed data).

### Crypt.publicEncryptFile / privateDecryptFile / privateEncryptFile / publicDecryptFile (key, fileIn, fileOut)

Read the whole `fileIn` into memory, process it as above and write `fileOut`
(created or replaced).

| Result | When |
|--------|------|
| `true` | `fileOut` written |
| `false` | `fileIn` missing or unreadable, the key cannot be read, decryption failed, or `fileOut` cannot be written; on a key or decryption error `fileOut` is **not** created or touched |
| `undefined` | `key` is not a `String` / `Buffer` |

## HTTPS

`HTTPS` has the same functions as the `HTTP` extension
(`quantum-script--http`), for `https://` URLs, over `OpenSSL.sslConnect`.

```javascript
var user = HTTPS.json("https://api.example.com/user", {id: 42});
var page = HTTPS.post("https://www.example.com/", "", null, "GET");
var ok = HTTPS.downloadFile("https://www.example.com/file.zip", "file.zip");
```

All functions use `this`: call them as `HTTPS.name(...)`, never through a
detached copy.

### Common behavior

1. **URL checks.** The scheme must be `https` (any case). Anything else
   (`http://`, `ftp://`) and a URL without `://` return `null`
   (`downloadFile`: `false`). An empty host returns `null` / `false`.
2. **Address.** The connection goes to `URL.getHostNameAndPort(url)`, with
   `:443` added when there is no port (also for an IPv6 address:
   `https://[::1]/` connects to `[::1]:443`). Name resolution, connect and the
   TLS handshake block with no timeout. A failed connection or handshake
   returns `null` / `false`.
3. **Request.** `MODE path?query HTTP/1.0`, `Host: <host[:port] as written>`,
   the function's headers and yours, an empty line, the body. `mode` is
   upper-cased; the default is `POST` (`json`, `post`, `postRequest`) or
   `GET` (`downloadFile`). Nothing in the URL is encoded for you
   (`URL.encodeComponent`). User info in the URL is not sent.
4. **Response.** Header lines are read with `readLn(1024)` until the empty
   line. Only `Content-Length` (any case) is used; the status line and the
   other headers are ignored. The body is read into `HTTPS.buffer` until
   `Content-Length` bytes arrived or the server closed the connection.
5. **No status code check, no redirects, no chunked bodies.** A `404` page is
   returned or saved like a `200` one. The request is HTTP/1.0, so servers
   do not use chunked transfer.

### The `headers` argument

One string, lines joined with `"\r\n"`, **never ending with `"\r\n"`**
(it would end the header block early). Header order:

| Function | Header order |
|----------|--------------|
| `json` | `Host`, *headers*, `Connection: close`, `Content-Type: application/json`, `Content-Length` |
| `post` | `Host`, *headers*, `Connection: close`, `Content-Type: text/plain` (only when *headers* has no `Content-Type:`, case sensitive), `Content-Length` |
| `postRequest` | `Host`, *headers*, `Connection: close`, `Content-Type: multipart/form-data; boundary=...`, `Content-Length` |
| `downloadFile` | `Host`, `Connection: close`, *headers* (nothing else) |

### HTTPS.json(url, data, headers, mode)

Sends `JSON.encode(data)` (always, also for `GET`; missing data is `null`)
and returns `JSON.decode` of the body.

| Result | When |
|--------|------|
| decoded value | the body is JSON |
| `undefined` | the body is empty or not JSON (error pages, redirects) |
| `null` | not an `https://` URL, no host, connection or handshake failed — or a JSON body `null` |

`JSON.encode` writes non-ASCII bytes as `\xHH` escapes, which strict JSON
parsers reject; for servers outside the XYO stack send standard JSON text
with `HTTPS.post(url, text, "Content-Type: application/json")`.

### HTTPS.post(url, data, headers, mode)

Sends the **String** `data` (its byte length is the `Content-Length`) and
returns the response body as a String, or `null` when the URL is not
`https://` or the connection failed. Give your own `Content-Type` in
`headers` to replace `text/plain`. For a `GET`: `HTTPS.post(url, "", null, "GET")`.

### HTTPS.postRequest(url, data, headers, mode)

Sends `data` as one multipart field named `request`, in a fixed layout
shared with `HTTP.postRequest`: delimiter lines are the boundary text without
the leading `--` and there is no closing `--`. Only for servers written for
it; for standard multipart build the body yourself and use `HTTPS.post`.
Returns the body String or `null`.

### HTTPS.downloadFile(url, fileName, headers, mode, extra, content)

Writes the response body to `fileName` (created or replaced) and returns
`true`, or `false` when the URL is not `https://`, the connection failed or
the file cannot be created.

| Argument | Meaning |
|----------|---------|
| `fileName` | target file; when `null` / `undefined` the last part of the URL path is used (`Shell` is loaded for that); a URL path ending in `/` gives no name and `false` |
| `headers` | optional extra header lines |
| `mode` | optional method, default `"GET"` |
| `extra` | **ignored**, kept for compatibility (`HTTP.downloadFile` has no such argument) |
| `content` | optional request body, written as is after the headers; no `Content-Length` is added, put it in `headers` |

`true` only means a response was saved: a `404` page is saved too, and a
connection that breaks during the body leaves a short file. Check the size
or a hash ([Recipes](recipes.md#verified-download)).

### HTTPS.decodeHeaderLine(headerLine), HTTPS.buffer

`decodeHeaderLine("Content-Length: 12\r\n")` returns `["Content-Length", "12"]`
(value trimmed), `false` when the line has no `:` after the first character.
`HTTPS.buffer` is the 128 KB `Buffer` every call reads the body into.

## OpenSSL.sslConnect(server)

Opens a TLS connection and returns an object, or `undefined` when the name
cannot be resolved, the connection is refused or the handshake fails.

```javascript
var socket = OpenSSL.sslConnect("www.example.com:443");
if (Script.isUndefined(socket)) {
	throw(new Error("cannot connect"));
};
socket.writeLn("HEAD / HTTP/1.0");
socket.writeLn("Host: www.example.com");
socket.writeLn("");
var status = socket.readLn(1024);      // "HTTP/1.1 200 OK\r\n"
socket.close();
```

`server` is `"host:port"` or `"[ipv6]:port"`; the port is required. The
returned object captures its own context and connection:

| Method | Calls | Result |
|--------|-------|--------|
| `isValid()` | | `true` |
| `read(size)` | `bioRead` | String, up to `size` bytes |
| `readLn(size)` | `bioReadLn` | one line with its end, at most `size` bytes |
| `write(str)` | `bioWrite` | bytes written |
| `writeLn(str)` | `bioWriteLn` | bytes written, `str + "\r\n"` |
| `readToBuffer(buffer, size)` | `bioReadToBuffer` | bytes read into `buffer` |
| `writeFromBuffer(buffer)` | `bioWriteFromBuffer` | bytes written |
| `close()` | `bioClose` | sends the TLS close notification |

## Low level TLS: ctx, bio

`OpenSSL.sslConnect` is built on these; use them directly only when you need
something it does not do.

| Function | Behavior |
|----------|----------|
| `ctxNew()` | new client `SSL_CTX` (no certificate verification); `undefined` if OpenSSL fails |
| `sslNew(ctx)` | new `SSL` object on `ctx`; not used by the other functions |
| `bioConnect(ctx, server)` | connect to `server` (`"host:port"`), send SNI, complete the TLS handshake; `undefined` on failure |
| `bioClose(bio)` | TLS shutdown (`close_notify`); data already received can still be read; the socket is freed when the value is collected or with `bio.close()` (after which the handle is invalid) |
| `bioRead(bio, size)` | reads up to `size` bytes; stops early when less data is available at once or the connection ends; `""` at the end of the stream; `undefined` when `size` is missing, negative, `NaN` or infinite |
| `bioReadLn(bio, size)` | reads one line, returned **with** its `"\r\n"` (a lone `"\r"` stays in the line); stops after `size` bytes; returns what was read when the connection ends; `undefined` when the connection ended before any byte; `""` for `size` < 1 |
| `bioWrite(bio, str)` | writes the bytes of `str`; returns the count (`<= 0` on error) |
| `bioWriteLn(bio, str)` | writes `str + "\r\n"` |
| `bioReadToBuffer(bio, buffer, size)` | reads until `size` bytes (capped at `buffer.size`; `Infinity` = `buffer.size`) or the end of the stream; sets `buffer.length`; returns the count, `0` at the end; `size` missing, `NaN`, `0` or negative: `0` |
| `bioWriteFromBuffer(bio, buffer)` | writes the first `buffer.length` bytes |

## Hashes

### OpenSSL.sha256(bufferIn, start, length, hashOut), OpenSSL.sha512(...)

Hashes `length` bytes of `bufferIn` from `start` into `hashOut`, which is
enlarged if smaller than the digest, and sets `hashOut.length` to 32 / 64.

```javascript
var data = Buffer.fromString("abc");
var hash = new Buffer(OpenSSL.sha256Length);
OpenSSL.sha256(data, 0, data.length, hash);
hash.toHex();   // "ba7816bf..."
```

Returns `false` (and leaves `hashOut` unchanged) when `length` is `0` or the
range `start .. start + length` is outside `bufferIn.length`. Throws
`invalid parameter` when `bufferIn` or `hashOut` is not a `Buffer`. For a
whole string or file, the `SHA256` / `SHA512` extensions are simpler.

## Raw RSA

| Function | Behavior |
|----------|----------|
| `rsaPublicKey(buffer)` | reads a `BEGIN PUBLIC KEY` PEM from the bytes of `buffer`; `null` if it cannot |
| `rsaPrivateKey(buffer)` | reads an unencrypted private key PEM; `null` if it cannot |
| `rsaSize(rsa)` | modulus size in bytes (256 for 2048 bits) |
| `rsaPublicEncrypt(rsa, buffer, bufferOut)` | encrypts `buffer` (1 to `rsaSize - 11` bytes) into `bufferOut` (`rsaSize` bytes) |
| `rsaPrivateEncrypt(rsa, buffer, bufferOut)` | same with the private key (raw RSA signature block) |
| `rsaPrivateDecrypt(rsa, buffer, bufferOut)` | decrypts an `rsaSize` block, `bufferOut.length` = message length |
| `rsaPublicDecrypt(rsa, buffer, bufferOut)` | same with the public key |

`bufferOut` is enlarged to `rsaSize` when smaller. The four functions return
`true`, or `false` with `bufferOut` zeroed and `bufferOut.length` 0 (data too
long, wrong block, wrong key kind). An empty input throws `invalid length`.
`rsaPrivateEncrypt` / `rsaPublicEncrypt` need the matching key kind: a public
key cannot do private operations.

## Errors

| Message | Cause |
|---------|-------|
| `invalid parameter` | a `Buffer` argument that is not a `Buffer`; a handle that is not the expected kind or was closed |
| `invalid length` | `rsa*Encrypt` / `rsa*Decrypt` with an empty input buffer |
| `OpenSSL initialization` | OpenSSL could not be initialized when loading |
| `Unable to open "OpenSSL"` | the extension (or one of its dependencies, named in the message) was not found |

`HTTPS.*`, `sslConnect` and the `Crypt.*` functions report failures with
their result (`null`, `false`, `undefined`), not with exceptions.
