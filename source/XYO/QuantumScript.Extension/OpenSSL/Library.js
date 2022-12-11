// Quantum Script Extension OpenSSL
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

Script.requireExtension("Socket");
Script.requireExtension("File");
Script.requireExtension("URL");
Script.requireExtension("JSON");

OpenSSL.sslConnect = function(server) {
	var fn = function(server) {
		var ctx = OpenSSL.ctxNew();
		var bio = OpenSSL.bioConnect(ctx, server);
		this.isValid = function() {
			return !Script.isNil(bio);
		};
		this.close = function() {
			OpenSSL.bioClose(bio);
		};
		this.read = function(size) {
			return OpenSSL.bioRead(bio, size);
		};
		this.readLn = function(size) {
			return OpenSSL.bioReadLn(bio, size);
		};
		this.write = function(str) {
			return OpenSSL.bioWrite(bio, str);
		};
		this.writeLn = function(str) {
			return OpenSSL.bioWriteLn(bio, str);
		};
		this.readToBuffer = function(buffer, size) {
			return OpenSSL.bioReadToBuffer(bio, buffer, size);
		};
		this.writeFromBuffer = function(buffer) {
			return OpenSSL.bioWriteFromBuffer(bio, buffer);
		};
	};
	var retV = new fn(server);
	if(retV.isValid()) {
		return retV;
	};
	return undefined;
};

if(Script.isNil(HTTPS)) {
	HTTPS = {};
};

HTTPS.buffer = new Buffer(128 * 1024);

HTTPS.decodeHeaderLine = function(headerLine) {
	var x = headerLine.indexOf(":");
	if(x > 0) {
		return [headerLine.substring(0, x), headerLine.substring(x + 1).trim()];
	};
	return false;
};

HTTPS.json = function(url, data, headers, mode) {
	var socket;
	var decodedResponse;
	var contentLength;
	var host;
	var json = JSON.encode(data);
	var response;
	var header;

	if(URL.getSchemeName(url).toUpperCaseAscii() != "HTTPS") {
		return null;
	};
	host = URL.getHostNameAndPort(url);
	if(!host) {
		return null;
	};

	if(host.indexOf(":") < 0) {
		host += ":443";
	};

	if(Script.isNil(mode)) {
		mode = "POST";
	};
	mode = mode.toUpperCaseAscii();

	socket = OpenSSL.sslConnect(host);

	if(socket) {
		contentLength = Infinity;
		if(URL.getQuery(url)) {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + "?" + URL.getQuery(url) + " HTTP/1.1");
		} else {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + " HTTP/1.1");
		};
		socket.writeLn("Host: " + URL.getHostNameAndPort(url));
		if(!Script.isNil(headers)) {
			socket.writeLn(headers);
		};
		socket.writeLn("Connection: close");
		socket.writeLn("Content-Type: openssl/json");
		socket.writeLn("Content-Length: " + json.length);
		socket.writeLn("");
		socket.write(json);
		for(;;) {
			response = socket.readLn(1024);
			if(Script.isUndefined(response)) {
				break;
			};
			if(response.length == 0) {
				break;
			};
			if(response === "\r\n") {
				break;
			};
			decodedResponse = this.decodeHeaderLine(response);
			if(decodedResponse) {
				if(decodedResponse[0] == "Content-Length") {
					contentLength = Convert.toNumber(decodedResponse[1]);
				};
			};
		};
		json = "";
		while(contentLength) {
			if(socket.readToBuffer(this.buffer, contentLength)) {
				json += Convert.toString(this.buffer);
				contentLength -= this.buffer.length;
			} else {
				break;
			};
		};
		socket.close();
		return JSON.decode(json);
	};
	return null;
};

HTTPS.downloadFile = function(url, fileName, headers, mode, extra, content) {
	var socket;
	var decodedResponse;
	var contentLength;
	var host;
	var response;
	var file;

	if(Script.isNil(fileName)) {
		fileName = Shell.getFileName(URL.getPathAndFileName(url));
	};

	if(URL.getSchemeName(url).toUpperCaseAscii() != "HTTPS") {
		return false;
	};

	host = URL.getHostNameAndPort(url);
	if(!host) {
		return false;
	};

	if(host.indexOf(":") < 0) {
		host += ":443";
	};

	if(Script.isNil(mode)) {
		mode = "GET";
	};
	mode = mode.toUpperCaseAscii();

	socket = OpenSSL.sslConnect(host);

	if(socket) {
		contentLength = Infinity;
		if(URL.getQuery(url)) {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + "?" + URL.getQuery(url) + " HTTP/1.1");
		} else {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + " HTTP/1.1");
		};
		socket.writeLn("Host: " + URL.getHostNameAndPort(url));
		socket.writeLn("Connection: close");
		if(!Script.isNil(headers)) {
			socket.writeLn(headers);
		};
		socket.writeLn("");
		if(!Script.isNil(content)) {
			socket.write(content);
		};
		for(;;) {
			response = socket.readLn(1024);
			if(Script.isUndefined(response)) {
				break;
			};
			if(response.length == 0) {
				break;
			};
			if(response === "\r\n") {
				break;
			};
			decodedResponse = this.decodeHeaderLine(response);
			if(decodedResponse) {
				if(decodedResponse[0] == "Content-Length") {
					contentLength = Convert.toNumber(decodedResponse[1]);
				};
			};
		};
		file = new File();
		if(file.openWrite(fileName)) {
			while(contentLength) {
				if(socket.readToBuffer(this.buffer, contentLength)) {
					file.writeFromBuffer(this.buffer);
					contentLength -= this.buffer.length;
				} else {
					break;
				};
			};
			file.close();
			return true;
		};
	};
	return false;
};


HTTPS.post = function(url, data, headers, mode) {
	var socket = new Socket();
	var decodedResponse;
	var contentLength;
	var host;
	var response;
	var header;

	if(URL.getSchemeName(url).toUpperCaseAscii() != "HTTPS") {
		return null;
	};
	host = URL.getHostNameAndPort(url);
	if(!host) {
		return null;
	};

	if(host.indexOf(":") < 0) {
		host += ":443";
	};

	if(Script.isNil(mode)) {
		mode = "POST";
	};
	mode = mode.toUpperCaseAscii();

	socket = OpenSSL.sslConnect(host);

	if(socket) {
		contentLength = Infinity;
		if(URL.getQuery(url)) {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + "?" + URL.getQuery(url) + " HTTP/1.1");
		} else {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + " HTTP/1.1");
		};
		socket.writeLn("Host: " + URL.getHostNameAndPort(url));
		if(!Script.isNil(headers)) {
			socket.writeLn(headers);
		};

		socket.writeLn("Connection: close");
		if(Script.isNil(headers)) {
			socket.writeLn("Content-Type: text/plain");
		} else {
			if(headers.indexOf("Content-Type:") < 0) {
				socket.writeLn("Content-Type: text/plain");
			};
		};
		socket.writeLn("Content-Length: " + data.length + "\r\n");
		socket.write(data);

		for(;;) {
			response = socket.readLn(1024);
			if(Script.isUndefined(response)) {
				break;
			};
			if(response.length == 0) {
				break;
			};
			if(response === "\r\n") {
				break;
			};
			decodedResponse = this.decodeHeaderLine(response);
			if(decodedResponse) {
				if(decodedResponse[0] == "Content-Length") {
					contentLength = Convert.toNumber(decodedResponse[1]);
				};
			};
		};
		data = "";
		while(contentLength) {
			if(socket.readToBuffer(this.buffer, contentLength)) {
				data += Convert.toString(this.buffer);
				contentLength -= this.buffer.length;
			} else {
				break;
			};
		};
		socket.close();
		return data;
	};
	return null;
};

HTTPS.postRequest = function(url, data, headers, mode) {
	var socket = new Socket();
	var decodedResponse;
	var contentLength;
	var host;
	var response;
	var header;

	if(URL.getSchemeName(url).toUpperCaseAscii() != "HTTPS") {
		return null;
	};
	host = URL.getHostNameAndPort(url);
	if(!host) {
		return null;
	};

	if(host.indexOf(":") < 0) {
		host += ":443";
	};

	if(Script.isNil(mode)) {
		mode = "POST";
	};
	mode = mode.toUpperCaseAscii();

	socket = OpenSSL.sslConnect(host);

	if(socket) {
		contentLength = Infinity;
		if(URL.getQuery(url)) {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + "?" + URL.getQuery(url) + " HTTP/1.1");
		} else {
			socket.writeLn(mode + " " + URL.getPathAndFileName(url) + " HTTP/1.1");
		};
		socket.writeLn("Host: " + URL.getHostNameAndPort(url));
		if(!Script.isNil(headers)) {
			socket.writeLn(headers);
		};

		var strBoundary = "---===68b0cd10b99337fb5ae7bf88dd0c34e39ce26281ae8351a70ed0a0394e1a56b4";
		var strContentDisposition = "Content-Disposition: form-data; name=\"request\"";

		socket.writeLn("Connection: close");
		socket.writeLn("Content-Type: multipart/form-data; boundary=" + strBoundary);
		socket.writeLn("Content-Length: " + (data.length + (strBoundary.length) * 2 + strContentDisposition.length + 2 + 4) + "\r\n");
		socket.writeLn(strBoundary);
		socket.writeLn(strContentDisposition + "\r\n");
		socket.write(data);
		socket.write(strBoundary);

		for(;;) {
			response = socket.readLn(1024);
			if(Script.isUndefined(response)) {
				break;
			};
			if(response.length == 0) {
				break;
			};
			if(response === "\r\n") {
				break;
			};
			decodedResponse = this.decodeHeaderLine(response);
			if(decodedResponse) {
				if(decodedResponse[0] == "Content-Length") {
					contentLength = Convert.toNumber(decodedResponse[1]);
				};
			};
		};
		data = "";
		while(contentLength) {
			if(socket.readToBuffer(this.buffer, contentLength)) {
				data += Convert.toString(this.buffer);
				contentLength -= this.buffer.length;
			} else {
				break;
			};
		};
		socket.close();
		return data;
	};
	return null;
};

