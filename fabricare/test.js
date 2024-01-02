// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("test");

// ---

for(var k=1;k<=1;++k){
	exitIf(Shell.execute("quantum-script  --execution-time test/test.000"+k+".js"));
};

