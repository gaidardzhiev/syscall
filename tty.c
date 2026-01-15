/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

int z(int y, int x, int w, int v) {
	register int r7 asm("r7") = y;
	register int r0 asm("r0") = x;
	register int r1 asm("r1") = w;
	register int r2 asm("r2") = v;
	asm volatile ("svc #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
	return r0;
}


void _ep() {
	char f[512];
	int e = z(54, 0, 0x5401, (int)f);
	if (e < 0) {
		const char *d = "not a tty...\n";
		register int r7w asm("r7") = 4;
		register int r0w asm("r0") = 1;
		register int r1w asm("r1") = (int)d;
		register int r2w asm("r2") = 10;
		asm volatile ("svc #0" : "=r" (r0w) : "r" (r7w), "r" (r0w), "r" (r1w), "r" (r2w) : "memory");
		register int r7x asm("r7") = 1;
		register int r0x asm("r0") = 1;
		asm volatile ("svc #0" : "=r" (r0x) : "r" (r7x), "r" (r0x) : "memory");
		__builtin_unreachable();
	}
	const char *c = "/proc/self/fd/0";
	int b = z(85, (int)c, (int)f, 512-1);
	if (b < 0) {
		const char *a = "unknown...\n";
		register int r7w2 asm("r7") = 4;
		register int r0w2 asm("r0") = 1;
		register int r1w2 asm("r1") = (int)a;
		register int r2w2 asm("r2") = 8;
		asm volatile ("svc #0" : "=r" (r0w2) : "r" (r7w2), "r" (r0w2), "r" (r1w2), "r" (r2w2) : "memory");
		register int r7x2 asm("r7") = 1;
		register int r0x2 asm("r0") = 1;
		asm volatile ("svc #0" : "=r" (r0x2) : "r" (r7x2), "r" (r0x2) : "memory");
		__builtin_unreachable();
	}
	f[b] = '\n';
	register int r7w3 asm("r7") = 4;
	register int r0w3 asm("r0") = 1;
	register int r1w3 asm("r1") = (int)f;
	register int r2w3 asm("r2") = b + 1;
	asm volatile ("svc #0" : "=r" (r0w3) : "r" (r7w3), "r" (r0w3), "r" (r1w3), "r" (r2w3) : "memory");
	register int r7x3 asm("r7") = 1;
	register int r0x3 asm("r0") = 0;
	asm volatile ("svc #0" : "=r" (r0x3) : "r" (r7x3), "r" (r0x3) : "memory");
}
