// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

// Network tests: needs access to the internet

Script.requireExtension("Console");
Script.requireExtension("Shell");
Script.requireExtension("OpenSSL");

function check(name, value, expected) {
	if (value !== expected) {
		Console.writeLn("test " + name + ": [" + value + "], expected [" + expected + "]");
		throw "test 02 failed";
	};
};

function statusLine(server, hostName) {
	var socket = OpenSSL.sslConnect(server);
	var line;
	if (Script.isUndefined(socket)) {
		return "connect failed";
	};
	socket.writeLn("HEAD / HTTP/1.0");
	socket.writeLn("Host: " + hostName);
	socket.writeLn("Connection: close");
	socket.writeLn("");
	line = socket.readLn(1024);
	socket.close();
	if (Script.isUndefined(line)) {
		return "no response";
	};
	return line.substring(0, 9);
};

// download a page
Shell.removeFile("index.html");
check("downloadFile", HTTPS.downloadFile("https://www.google.ro/", "index.html"), true);
check("downloadFile.size", Shell.getFileSize("index.html") > 0, true);
Shell.removeFile("index.html");

// default file name from the URL
Shell.removeFile("robots.txt");
check("downloadFile.default-name", HTTPS.downloadFile("https://www.google.ro/robots.txt"), true);
check("downloadFile.default-name.size", Shell.getFileSize("robots.txt") > 0, true);
Shell.removeFile("robots.txt");

// response as text
check("post.get", HTTPS.post("https://www.google.ro/robots.txt", "", null, "GET").indexOf("User-agent") >= 0, true);

// servers that need Server Name Indication
check("sni.example.com", statusLine("example.com:443", "example.com").substring(0, 7), "HTTP/1.");
check("sni.cloudflare", statusLine("www.cloudflare.com:443", "www.cloudflare.com").substring(0, 7), "HTTP/1.");

Console.writeLn("-> test 02 ok");
