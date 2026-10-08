# Quantum Script Extension OpenSSL

Quantum Script extension
- `https://` client for scripts: send JSON, post text, download files
(`HTTPS.json`, `HTTPS.post`, `HTTPS.postRequest`, `HTTPS.downloadFile`).
- TLS connections to any server, with line and buffer I/O
(`OpenSSL.sslConnect`).
- RSA hybrid encryption of data and files with PEM keys
(`Crypt.publicEncrypt`, `Crypt.privateDecrypt`, ...).
- SHA-256 / SHA-512 and raw RSA (PKCS #1 v1.5) on buffers.

```javascript
Script.requireExtension("OpenSSL");

OpenSSL;
OpenSSL.sha256Length;
OpenSSL.sha512Length;
OpenSSL.ctxNew();
OpenSSL.sslNew(ctx);
OpenSSL.bioConnect(ctx,server);
OpenSSL.bioClose(bio);
OpenSSL.bioRead(bio,size);
OpenSSL.bioReadLn(bio,size);
OpenSSL.bioWrite(bio,str);
OpenSSL.bioWriteLn(bio,str);
OpenSSL.bioReadToBuffer(bio,buffer,size);
OpenSSL.bioWriteFromBuffer(bio,buffer);
OpenSSL.sha256(bufferIn,st,ln,hashOut);
OpenSSL.sha512(bufferIn,st,ln,hashOut);
OpenSSL.rsaPublicKey(buffer);
OpenSSL.rsaPrivateKey(buffer);
OpenSSL.rsaPublicEncrypt(rsa,buffer,bufferOut);
OpenSSL.rsaPublicDecrypt(rsa,buffer,bufferOut);
OpenSSL.rsaPrivateEncrypt(rsa,buffer,bufferOut);
OpenSSL.rsaPrivateDecrypt(rsa,buffer,bufferOut);
OpenSSL.rsaSize(rsa);
OpenSSL.sslConnect(server);
HTTPS.buffer;
HTTPS.decodeHeaderLine(headerLine);
HTTPS.json(url,data,headers,mode);
HTTPS.post(url,data,headers,mode);
HTTPS.postRequest(url,data,headers,mode);
HTTPS.downloadFile(url,fileName,headers,mode,extra,content);
Crypt.privateEncrypt(privateKey,data);
Crypt.privateDecrypt(privateKey,data);
Crypt.publicEncrypt(publicKey,data);
Crypt.publicDecrypt(publicKey,data);
Crypt.privateEncryptFile(key,fileIn,fileOut);
Crypt.privateDecryptFile(key,fileIn,fileOut);
Crypt.publicEncryptFile(key,fileIn,fileOut);
Crypt.publicDecryptFile(key,fileIn,fileOut);
```

Built on OpenSSL (`libssl`, `libcrypto`), `quantum-script--crypt` and
`xyo-cryptography`, part of the XYO C++ SDK. Server certificates are **not**
verified: see [Script API, Security](docs/script-api.md#security).

## Documentation

- [Overview](docs/README.md) - purpose, warnings, design
- [Getting started](docs/getting-started.md) - build, test, load from a script, hosts, register in a C++ host, static builds, threads
- [Script API](docs/script-api.md) - every function, keys, encrypted data format, HTTPS requests, security
- [Recipes](docs/recipes.md) - keys, encrypting messages and files, HTTPS JSON, verified downloads, TLS line protocols, local testing
- [C++ API](docs/cpp-api.md) - `registerInternalExtension`, the C++ RSA hybrid functions, data format, notes for maintainers
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--openssl](.claude/skills/quantum-script--openssl/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
