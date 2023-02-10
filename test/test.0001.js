// Public domain
// http://unlicense.org/

Script.requireExtension("Console");
Script.requireExtension("OpenSSL");

HTTPS.downloadFile("https://www.google.ro/", "test/index.html");

Console.writeLn("-> test 0001 ok");

