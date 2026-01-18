/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

struct T {
	unsigned int s;
	unsigned int n;
};

void _ep() {
	int p;
	char **q;
	asm volatile (
		"mov %0, r0\n\t"
		"mov %1, r1\n\t"
		: "=r"(p), "=r"(q)
		:
		: "memory"
	);
	if (p < 2) {
		const char *d = "usage: ./sleep <sec>\n";
		register int r7w asm("r7") = 4;
		register int r0w asm("r0") = 2;
		register int r1w asm("r1") = (int)d;
		register int r2w asm("r2") = 20;
		asm volatile ("svc 0" : "=r" (r0w) : "r" (r7w), "r" (r0w), "r" (r1w), "r" (r2w) : "memory");
		register int r7x asm("r7") = 1;
		register int r0x asm("r0") = 1;
		asm volatile ("svc 0" : "=r" (r0x) : "r" (r7x), "r" (r0x) : "memory");
		__builtin_unreachable();
	}
	unsigned int z = 0;
	const char *m = q[1];
	while (*m) {
		char o = *m++;
		if (o < '0' || o > '9') break;
		z = z * 10 + (o - '0');
	}
	if (z == 0) {
		const char *a = "invalid time interval...\n";
		register int r7w2 asm("r7") = 4;
		register int r0w2 asm("r0") = 2;
		register int r1w2 asm("r1") = (int)a;
		register int r2w2 asm("r2") = 24;
		asm volatile ("svc 0" : "=r" (r0w2) : "r" (r7w2), "r" (r0w2), "r" (r1w2), "r" (r2w2) : "memory");
		register int r7x2 asm("r7") = 1;
		register int r0x2 asm("r0") = 2;
		asm volatile ("svc 0" : "=r" (r0x2) : "r" (r7x2), "r" (r0x2) : "memory");
		__builtin_unreachable();
	}
	struct T ts = { z, 0 };
	register int r7s asm("r7") = 162;
	register int r0s asm("r0") = (int)&ts;
	register int r1s asm("r1") = 0;
	asm volatile("svc 0" : "=r"(r0s) : "r"(r7s), "r"(r0s), "r"(r1s) : "memory");
	register int r7x3 asm("r7") = 1;
	register int r0x3 asm("r0") = 0;
	asm volatile ("svc 0" : "=r" (r0x3) : "r" (r7x3), "r" (r0x3) : "memory");
}
