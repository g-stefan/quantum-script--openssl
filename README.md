# Quantum Script Extension OpenSSL

```javascript
OpenSSL;
OpenSSL.ctxNew();
OpenSSL.sslNew(ctx);
OpenSSL.bioConnect(ctx,server);
OpenSSL.bioClose(bio);
OpenSSL.bioRead(bio);
OpenSSL.bioReadLn(bio);
OpenSSL.bioWrite(bio,str);
OpenSSL.bioWriteLn(bio,str);
OpenSSL.bioReadToBuffer(bio,buffer);
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
HTTPS.jsonRequest(url,data,headers,mode);
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

## License

Copyright (c) 2022 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
