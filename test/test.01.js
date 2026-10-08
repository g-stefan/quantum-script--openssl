// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

// Offline tests: hashes, RSA, Crypt, argument checks, HTTPS argument handling

Script.requireExtension("Console");
Script.requireExtension("Shell");
Script.requireExtension("OpenSSL");

var testPath = "../../test/";

function check(name, value, expected) {
	if (value !== expected) {
		Console.writeLn("test " + name + ": [" + value + "], expected [" + expected + "]");
		throw "test 01 failed";
	};
};

function errorOf(fn) {
	try {
		fn();
	} catch (e) {
		return e.message;
	};
	return "no error";
};

function readKey(name) {
	return Shell.fileGetContents(testPath + name);
};

var privateKey = readKey("test.private.pem");
var publicKey = readKey("test.public.pem");

// --- SHA

var hash = new Buffer(64);
var source = Buffer.fromString("abc");

check("sha256.length", OpenSSL.sha256Length, 32);
check("sha512.length", OpenSSL.sha512Length, 64);
check("sha256", OpenSSL.sha256(source, 0, 3, hash), true);
check("sha256.value", hash.toHex(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
check("sha512", OpenSSL.sha512(source, 0, 3, hash), true);
check("sha512.value", hash.toHex(), "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
check("sha256.part", OpenSSL.sha256(Buffer.fromString("xxabcxx"), 2, 3, hash), true);
check("sha256.part.value", hash.toHex(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
check("sha256.small-output", OpenSSL.sha256(source, 0, 3, new Buffer(4)), true);
check("sha256.empty", OpenSSL.sha256(source, 0, 0, hash), false);
check("sha256.out-of-range", OpenSSL.sha256(source, 2, 5, hash), false);
check("sha256.start-out-of-range", OpenSSL.sha256(source, 4, 1, hash), false);
check("sha512.out-of-range", OpenSSL.sha512(source, 1, 3, hash), false);
check("sha256.not-buffer", errorOf(function() { OpenSSL.sha256("abc", 0, 3, hash); }), "invalid parameter");

// --- RSA keys

var rsaPublic = OpenSSL.rsaPublicKey(Buffer.fromString(publicKey));
var rsaPrivate = OpenSSL.rsaPrivateKey(Buffer.fromString(privateKey));

check("rsa.public", typeof(rsaPublic), "Resource");
check("rsa.private", typeof(rsaPrivate), "Resource");
check("rsa.private.pkcs1", typeof(OpenSSL.rsaPrivateKey(Buffer.fromString(readKey("test.private.pkcs1.pem")))), "Resource");
check("rsa.public.pkcs1", OpenSSL.rsaPublicKey(Buffer.fromString(readKey("test.public.pkcs1.pem"))), null);
// encrypted keys are not supported, they must not ask for a password on the console
check("rsa.private.encrypted", OpenSSL.rsaPrivateKey(Buffer.fromString(readKey("test.private.encrypted.pem"))), null);
check("rsa.private.pkcs1.encrypted", OpenSSL.rsaPrivateKey(Buffer.fromString(readKey("test.private.pkcs1.encrypted.pem"))), null);
check("rsa.public.invalid", OpenSSL.rsaPublicKey(Buffer.fromString("not a key")), null);
check("rsa.private.invalid", OpenSSL.rsaPrivateKey(Buffer.fromString("not a key")), null);
check("rsa.public.not-buffer", errorOf(function() { OpenSSL.rsaPublicKey(publicKey); }), "invalid parameter");
check("rsa.size", OpenSSL.rsaSize(rsaPublic), 256);
check("rsa.size.private", OpenSSL.rsaSize(rsaPrivate), 256);

// --- RSA encrypt / decrypt

var encrypted = new Buffer(16);
var decrypted = new Buffer(16);

check("rsa.publicEncrypt", OpenSSL.rsaPublicEncrypt(rsaPublic, Buffer.fromString("hello"), encrypted), true);
check("rsa.publicEncrypt.length", encrypted.length, 256);
check("rsa.privateDecrypt", OpenSSL.rsaPrivateDecrypt(rsaPrivate, encrypted, decrypted), true);
check("rsa.privateDecrypt.value", decrypted.toString(), "hello");

check("rsa.privateEncrypt", OpenSSL.rsaPrivateEncrypt(rsaPrivate, Buffer.fromString("world"), encrypted), true);
check("rsa.privateEncrypt.length", encrypted.length, 256);
check("rsa.publicDecrypt", OpenSSL.rsaPublicDecrypt(rsaPublic, encrypted, decrypted), true);
check("rsa.publicDecrypt.value", decrypted.toString(), "world");

// PKCS #1 v1.5 padding: at most rsaSize - 11 bytes
var text245 = "";
for (var k = 0; k < 245; ++k) {
	text245 += "a";
};
check("rsa.publicEncrypt.max", OpenSSL.rsaPublicEncrypt(rsaPublic, Buffer.fromString(text245), encrypted), true);
check("rsa.publicEncrypt.too-long", OpenSSL.rsaPublicEncrypt(rsaPublic, Buffer.fromString(text245 + "a"), encrypted), false);
check("rsa.publicEncrypt.too-long.length", encrypted.length, 0);
check("rsa.publicEncrypt.empty", errorOf(function() { OpenSSL.rsaPublicEncrypt(rsaPublic, new Buffer(16), encrypted); }), "invalid length");
// OpenSSL 1.1 reports an error, OpenSSL 3.2+ (implicit rejection) returns random bytes
check("rsa.privateDecrypt.garbage", Script.isBoolean(OpenSSL.rsaPrivateDecrypt(rsaPrivate, Buffer.fromString("garbage"), decrypted)), true);

// --- Resources of the wrong kind

var context = OpenSSL.ctxNew();
check("ctx", typeof(context), "Resource");
check("ssl", typeof(OpenSSL.sslNew(context)), "Resource");
check("resource.undefined", errorOf(function() { OpenSSL.rsaSize(undefined); }), "invalid parameter");
check("resource.number", errorOf(function() { OpenSSL.rsaSize(1); }), "invalid parameter");
check("resource.kind", errorOf(function() { OpenSSL.rsaSize(context); }), "invalid parameter");
check("resource.rsa-as-bio", errorOf(function() { OpenSSL.bioClose(rsaPublic); }), "invalid parameter");
check("resource.rsa-as-ctx", errorOf(function() { OpenSSL.sslNew(rsaPublic); }), "invalid parameter");
check("resource.bioRead", errorOf(function() { OpenSSL.bioRead(null, 10); }), "invalid parameter");
check("resource.bioWrite", errorOf(function() { OpenSSL.bioWrite(context, "x"); }), "invalid parameter");
check("resource.rsaPublicEncrypt", errorOf(function() { OpenSSL.rsaPublicEncrypt("key", Buffer.fromString("x"), encrypted); }), "invalid parameter");

// --- Crypt with RSA keys

var data;
var data2;

data = Crypt.publicEncrypt(publicKey, "hello");
check("crypt.publicEncrypt", typeof(data), "Buffer");
check("crypt.publicEncrypt.length", data.length, 256 + 200);
data2 = Crypt.publicEncrypt(publicKey, "hello");
check("crypt.publicEncrypt.random", data.toHex() != data2.toHex(), true);
check("crypt.privateDecrypt", Crypt.privateDecrypt(privateKey, data).toString(), "hello");
check("crypt.privateDecrypt.2", Crypt.privateDecrypt(Buffer.fromString(privateKey), data2).toString(), "hello");

data = Crypt.privateEncrypt(privateKey, Buffer.fromString("world"));
check("crypt.privateEncrypt", typeof(data), "Buffer");
check("crypt.publicDecrypt", Crypt.publicDecrypt(Buffer.fromString(publicKey), data).toString(), "world");

data = Crypt.publicEncrypt(publicKey, "");
check("crypt.empty", Crypt.privateDecrypt(privateKey, data).toString(), "");

// tampered data
data = Crypt.publicEncrypt(publicKey, "hello");
data.setU8(300, (data.getU8(300) + 1) % 256);
check("crypt.tampered", Crypt.privateDecrypt(privateKey, data), undefined);

check("crypt.short", Crypt.privateDecrypt(privateKey, "short"), undefined);
check("crypt.short.public", Crypt.publicDecrypt(publicKey, "short"), undefined);
check("crypt.bad-key", Crypt.publicEncrypt("not a key", "hello"), undefined);
check("crypt.bad-key.private", Crypt.privateEncrypt("not a key", "hello"), undefined);
check("crypt.wrong-key-kind", Crypt.publicEncrypt(privateKey, "hello"), undefined);
check("crypt.key-not-string", Crypt.publicEncrypt(123, "hello"), undefined);
check("crypt.data-not-string", Crypt.publicEncrypt(publicKey, 123), undefined);

// --- Crypt files

var fileIn = "test.01.in.txt";
var fileEncrypted = "test.01.encrypted.bin";
var fileOut = "test.01.out.txt";

Shell.filePutContents(fileIn, "file content");

Shell.removeFile(fileEncrypted);
Shell.removeFile(fileOut);
check("crypt.publicEncryptFile", Crypt.publicEncryptFile(publicKey, fileIn, fileEncrypted), true);
check("crypt.privateDecryptFile", Crypt.privateDecryptFile(privateKey, fileEncrypted, fileOut), true);
check("crypt.privateDecryptFile.value", Shell.fileGetContents(fileOut), "file content");

Shell.removeFile(fileEncrypted);
Shell.removeFile(fileOut);
check("crypt.privateEncryptFile", Crypt.privateEncryptFile(privateKey, fileIn, fileEncrypted), true);
check("crypt.publicDecryptFile", Crypt.publicDecryptFile(publicKey, fileEncrypted, fileOut), true);
check("crypt.publicDecryptFile.value", Shell.fileGetContents(fileOut), "file content");

// a bad key must not report success nor write an empty file
Shell.removeFile(fileEncrypted);
check("crypt.publicEncryptFile.bad-key", Crypt.publicEncryptFile("not a key", fileIn, fileEncrypted), false);
check("crypt.publicEncryptFile.bad-key.file", Shell.fileExists(fileEncrypted), false);
check("crypt.privateEncryptFile.bad-key", Crypt.privateEncryptFile("not a key", fileIn, fileEncrypted), false);
check("crypt.privateEncryptFile.bad-key.file", Shell.fileExists(fileEncrypted), false);
check("crypt.privateDecryptFile.wrong", Crypt.privateDecryptFile(privateKey, fileIn, fileOut + ".x"), false);
check("crypt.encryptFile.missing", Crypt.publicEncryptFile(publicKey, "missing.file", fileEncrypted), false);

Shell.removeFile(fileIn);
Shell.removeFile(fileOut);

// --- HTTPS argument handling, no network

check("https.decodeHeaderLine", HTTPS.decodeHeaderLine("Content-Length: 12\r\n").join("|"), "Content-Length|12");
check("https.decodeHeaderLine.none", HTTPS.decodeHeaderLine("HTTP/1.0 200 OK\r\n"), false);
check("https.json.no-scheme", HTTPS.json("not a url", {}), null);
check("https.json.http", HTTPS.json("http://127.0.0.1/", {}), null);
check("https.post.no-scheme", HTTPS.post("not a url", "x"), null);
check("https.postRequest.no-scheme", HTTPS.postRequest("not a url", "x"), null);
check("https.downloadFile.no-scheme", HTTPS.downloadFile("not a url", "x.html"), false);
check("https.downloadFile.http", HTTPS.downloadFile("http://127.0.0.1/", "x.html"), false);
// default file name, loads Shell when needed; a path ending in "/" has no file name
check("https.downloadFile.no-file-name", HTTPS.downloadFile("https://127.0.0.1:1/"), false);
check("https.sslConnect.refused", OpenSSL.sslConnect("127.0.0.1:1"), undefined);

Console.writeLn("-> test 01 ok");
