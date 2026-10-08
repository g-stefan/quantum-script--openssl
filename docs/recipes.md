# Recipes

All examples start with:

```javascript
Script.requireExtension("Console");
Script.requireExtension("Shell");
Script.requireExtension("OpenSSL");
```

- [Create a key pair](#create-a-key-pair)
- [Encrypt a message for the key owner](#encrypt-a-message-for-the-key-owner)
- [Encrypt a file](#encrypt-a-file)
- [Store encrypted data as text](#store-encrypted-data-as-text)
- [Call a JSON API over HTTPS](#call-a-json-api-over-https)
- [Several headers, authentication](#several-headers-authentication)
- [Verified download](#verified-download)
- [A line protocol over TLS](#a-line-protocol-over-tls)
- [Read a whole response into a buffer](#read-a-whole-response-into-a-buffer)
- [Hash part of a buffer](#hash-part-of-a-buffer)
- [Test against a local TLS server](#test-against-a-local-tls-server)

## Create a key pair

The extension reads keys, it does not create them. Use the `openssl` command
line tool (in the SDK `bin` folder on Windows):

```bash
openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:3072 -out private.pem
openssl rsa -in private.pem -pubout -out public.pem
```

`private.pem` starts with `-----BEGIN PRIVATE KEY-----`, `public.pem` with
`-----BEGIN PUBLIC KEY-----`; both are accepted. Conversions for keys in
other forms:

| You have | Convert with |
|----------|--------------|
| `BEGIN RSA PUBLIC KEY` (PKCS #1 public key) | `openssl rsa -RSAPublicKey_in -in key.pem -pubout -out public.pem` |
| an encrypted private key | `openssl pkey -in key.pem -out private.pem` (asks for the password; protect the output file) |
| an OpenSSH private key | `ssh-keygen -p -m PEM -f id_rsa` (on a copy) |
| a certificate, need its public key | `openssl x509 -in cert.pem -pubkey -noout -out public.pem` |

Check a key from a script:

```javascript
var key = OpenSSL.rsaPublicKey(Buffer.fromString(Shell.fileGetContents("public.pem")));
if (Script.isNull(key)) {
	throw(new Error("not a BEGIN PUBLIC KEY RSA key"));
};
Console.writeLn("RSA " + (OpenSSL.rsaSize(key) * 8) + " bits");
```

## Encrypt a message for the key owner

The sender has only `public.pem`; the receiver keeps `private.pem`.

```javascript
// sender
var publicKey = Shell.fileGetContents("public.pem");
var encrypted = Crypt.publicEncrypt(publicKey, "meet at 10");
if (Script.isUndefined(encrypted)) {
	throw(new Error("cannot read public.pem"));
};
Shell.filePutContentsBuffer("message.bin", encrypted);

// receiver
var privateKey = Shell.fileGetContents("private.pem");
var message = Crypt.privateDecrypt(privateKey, Shell.fileGetContentsBuffer("message.bin"));
if (Script.isUndefined(message)) {
	throw(new Error("wrong key or damaged message"));
};
Console.writeLn(message.toString());
```

Every call uses a new random session key: encrypting the same text twice
gives different bytes.

## Encrypt a file

```javascript
var publicKey = Shell.fileGetContents("public.pem");
if (!Crypt.publicEncryptFile(publicKey, "report.pdf", "report.pdf.enc")) {
	throw(new Error("encryption failed"));
};

var privateKey = Shell.fileGetContents("private.pem");
if (!Crypt.privateDecryptFile(privateKey, "report.pdf.enc", "report.pdf")) {
	throw(new Error("wrong key or damaged file"));
};
```

There is no streaming: the whole input and the whole output are held in
memory, so the file size is limited by the available memory. On a key or
decryption error the function returns `false` and the output file is not
created.

## Store encrypted data as text

```javascript
Script.requireExtension("Base64");
var text = Base64.encode(Crypt.publicEncrypt(publicKey, "token"));     // for JSON, INI, e-mail (a Buffer is encoded as bytes)
var back = Crypt.privateDecrypt(privateKey, Base64.decodeToBuffer(text));
```

Or `encrypted.toHex()` and `Buffer.fromHex(text)`.

## Call a JSON API over HTTPS

```javascript
var result = HTTPS.json("https://api.example.com/v1/items?page=2", null, "Accept: application/json", "GET");
if (Script.isNull(result)) {
	throw(new Error("cannot connect"));
};
if (Script.isUndefined(result)) {
	throw(new Error("not JSON: error page or redirect"));
};
Console.writeLn(result.length + " items");
```

`HTTPS.json` always sends a body (`null` for a `GET`). If the server rejects
that, use `JSON.decode(HTTPS.post(url, "", headers, "GET"))`.

For servers outside the XYO stack send non-ASCII text as standard JSON (see
the `quantum-script--http` recipes, *Standard JSON for non-ASCII text*) with
`HTTPS.post(url, jsonText, "Content-Type: application/json")`.

## Several headers, authentication

```javascript
Script.requireExtension("Base64");
var headers = [
	"Authorization: Basic " + Base64.encode("user:password"),
	"Accept: application/json"
].join("\r\n");                                 // no "\r\n" at the end
var reply = HTTPS.post("https://api.example.com/v1/notes", "hello", headers);
```

Remember that the server certificate is not verified: send credentials only
on networks you trust.

## Verified download

`downloadFile` returns `true` for error pages and for broken transfers; check
the result against a hash published by a trusted source:

```javascript
Script.requireExtension("SHA256");

var url = "https://www.example.com/releases/app-1.0.0.zip";
var expected = "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08";

Shell.removeFile("app.zip.part");
if (!HTTPS.downloadFile(url, "app.zip.part")) {
	throw(new Error("cannot connect"));
};
if (SHA256.fileHash("app.zip.part") != expected) {
	Shell.removeFile("app.zip.part");
	throw(new Error("bad download"));
};
Shell.removeFile("app.zip");
Shell.rename("app.zip.part", "app.zip");
```

## A line protocol over TLS

`OpenSSL.sslConnect` works with any TLS server, not only HTTP:

```javascript
var socket = OpenSSL.sslConnect("smtp.example.com:465");
if (Script.isUndefined(socket)) {
	throw(new Error("cannot connect"));
};
var line = socket.readLn(1024);                 // "220 smtp.example.com ESMTP ...\r\n"
socket.writeLn("EHLO client.example.com");
for (;;) {
	line = socket.readLn(1024);
	if (Script.isUndefined(line)) {
		break;                                  // connection closed
	};
	Console.write(line);
	if (line.substring(3, 1) == " ") {
		break;                                  // last line of a multi line reply
	};
};
socket.writeLn("QUIT");
socket.close();
```

`readLn` returns the line with its `"\r\n"`, `undefined` when the
connection closed before any byte. Reads block: there is no timeout.

## Read a whole response into a buffer

```javascript
var socket = OpenSSL.sslConnect("www.example.com:443");
socket.writeLn("GET / HTTP/1.0");
socket.writeLn("Host: www.example.com");
socket.writeLn("");

var buffer = new Buffer(64 * 1024);
var response = "";
while (socket.readToBuffer(buffer, Infinity) > 0) {
	response += buffer.toString();
};
socket.close();
var status = response.substring(0, response.indexOf("\r\n"));   // "HTTP/1.1 200 OK", the server's version
```

This is how to get the status code and the response headers, which the
`HTTPS.*` functions ignore.

## Hash part of a buffer

```javascript
var data = Shell.fileGetContentsBuffer("archive.bin");
var hash = new Buffer(OpenSSL.sha512Length);
// skip a 16 byte header
if (OpenSSL.sha512(data, 16, data.length - 16, hash)) {
	Console.writeLn(hash.toHex());
};
```

## Test against a local TLS server

To see exactly what the `HTTPS` functions send, run a local server with a
self signed certificate (the certificate is accepted, since nothing is
verified):

```bash
openssl req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 1 -subj "/CN=localhost"
openssl s_server -accept 8443 -cert server.crt -key server.key -www
```

```javascript
Console.writeLn(HTTPS.post("https://localhost:8443/", "", null, "GET"));
```

`s_server -www` answers with a status page; use `-WWW` to serve files from
the current folder.
