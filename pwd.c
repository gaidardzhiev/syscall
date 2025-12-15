/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void _ep() {
	char b[4096];
	long ret;
	__asm__ volatile (
		"mov r7, %[g]\n"
		"mov r0, %[b]\n"
		"mov r1, %[s]\n"
		"svc #0\n"
		"mov %[r], r0\n"
		: [r] "=r" (ret)
		: [g] "I" (183), [b] "r" (b), [s] "I" (4096)
		: "r0", "r1", "r7", "memory"
	);
	if (ret < 0) {
		const char *er = "error\n";
		__asm__ volatile (
			"mov r7, %[w]\n"
			"mov r0, #2\n"
			"mov r1, %[m]\n"
			"mov r2, #6\n"
			"svc #0\n"
			:
			: [w] "I" (4), [m] "r" (er)
			: "r0", "r1", "r2", "r7", "memory"
		);
		__asm__ volatile (
			"mov r7, %[e]\n"
			"mov r0, #1\n"
			"svc #0\n"
			:
			: [e] "I" (1)
			: "r0", "r7"
		);
	}
	char *p = b;
	while (*p) p++;
	long len = p - b;
	__asm__ volatile (
		"mov r7, %[w]\n"
		"mov r0, #1\n"
		"mov r1, %[b]\n"
		"mov r2, %[l]\n"
		"svc #0\n"
		:
		: [w] "I" (4), [b] "r" (b), [l] "r" (len)
		: "r0", "r1", "r2", "r7", "memory"
	);
	const char nl = '\n';
	__asm__ volatile (
		"mov r7, %[w]\n"
		"mov r0, #1\n"
		"mov r1, %[nl]\n"
		"mov r2, #1\n"
		"svc #0\n"
		:
		: [w] "I" (4), [nl] "r" (&nl)
		: "r0", "r1", "r2", "r7", "memory"
	);
	__asm__ volatile (
		"mov r7, %[e]\n"
		"mov r0, #0\n"
		"svc #0\n"
		:
		: [e] "I" (1)
		: "r0", "r7"
	);
}
