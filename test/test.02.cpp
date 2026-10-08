// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2016-2026 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

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
	Extension::Shell::registerInternalExtension(executive);
	Extension::ShellFind::registerInternalExtension(executive);
	Extension::OpenSSL::registerInternalExtension(executive);
};

void test(int cmdN, char *cmdS[]) {

	const char *codeFile = "../../test/test.02.js";

	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		ExecutiveX::includePath(Shell::getFilePath(codeFile));
		if (ExecutiveX::executeFile(codeFile)) {
			ExecutiveX::endProcessing();
			return;
		};
		printf("%s\n", (ExecutiveX::getError()).value());
		printf("%s", (ExecutiveX::getStackTrace()).value());
		ExecutiveX::endProcessing();

		throw std::runtime_error("Code");
	};

	printf("%s\n", (ExecutiveX::getError()).value());
	throw std::runtime_error("initExecutive");
};

int main(int cmdN, char *cmdS[]) {
	try {

		test(cmdN, cmdS);

		return 0;

	} catch (const std::exception &e) {
		printf("* Error: %s\n", e.what());
	} catch (...) {
		printf("* Error: Unknown\n");
	};

	return 1;
};
