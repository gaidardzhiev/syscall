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
	register int p asm("r0");
	register char **q asm("r1");
	asm volatile ("" : "=r" (p), "=r" (q) : : );
	const char *m = "y";
	if (p > 1) {
		m = (const char *)q[1];
	}
	char nl = '\n';
	while (1) {
		fs(m);
		fw(4, (void*)1, &nl, 1);
	}
}
