/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void fw(int nr, void *z, void *x, int y) {
	register int r7 asm("r7") = nr;
	register void *r0 asm("r0") = z;
	register void *r1 asm("r1") = x;
	register int r2 asm("r2") = y;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
}

void fs(const char *s) {
	char b[256];
	int i;
	for (i = 0; i < 255 && s[i] != '\0'; i++) {
		b[i] = s[i];
	}
	b[i] = '\0';
	fw(4, (void*)1, b, i);
}

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
	char b[32];
	int n = p;
	int i = 0;
	if (n == 0) {
		b[i++] = '0';
	} else {
		char t[16];
		int j = 0;
		while (n > 0 && j < 16) {
			t[j++] = '0' + (n % 10);
			n /= 10;
		}
		while (j > 0) {
			b[i++] = t[--j];
		}
	}
	b[i++] = '\n';
	b[i] = '\0';
	fs(b);
	const char *m = "y";
	if (p > 1) {
		m = (const char *)q[1];
	}
	char nl = '\n';
	for (;;) {
		fs(m);
		fw(4, (void*)1, &nl, 1);
	}
}
