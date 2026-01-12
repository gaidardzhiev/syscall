/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void s(int x, void *y, void *z, void *j) {
	register int r7 asm("r7") = x;
	register void *r0 asm("r0") = y;
	register void *r1 asm("r1") = z;
	register void *r2 asm("r2") = j;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
}

int sr(int x, void *y, void *z, void *j) {
	register int r7 asm("r7") = x;
	register void *r0 asm("r0") = y;
	register void *r1 asm("r1") = z;
	register void *r2 asm("r2") = j;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
	register int r asm("r0");
	return r;
}

void w(int f, char *b, int l) {
	while (l > 0) {
		int r = sr(4, (void*)f,b,(void*)l);
		if (r <= 0) return;
		b += r;
		l -= r;
	}
}

int st(const char *s) {
	int i = 0;
	while (s[i]) i++;
	return i;
}

void e(const char *m, int l) {
	s(4, (void*)2, (void*)m, (void*)l);
}

void _ep() {
	char b[65536];
	int p, *q;
	asm volatile (
		"mov %0, r0\n\t"
		"mov %1, r1\n\t"
		: "=r"(p), "=r"(q)
		:
		: "memory"
	);
	if (p < 2) {
		for (;;) {
			int n = sr(3,(void*)0, b, (void*)65536);
			if (n <= 0) break;
			w(1, b, n);
		}
		s(1, (void*)0, 0, 0);
		return;
	}
	for (int i = 1; i < p; i++) {
		char *a = (char *)q[i];
		int f = sr(5, a, (void*)0, 0);
		if (f < 0) {
			const char *k = "no such file: ";
			e(k, 15);
			e(a, st(a));
			s(4, (void*)2, (void*)"\n", (void*)1);
			continue;
		}
		int n;
		for (;;) {
			n = sr(3, (void*)f, b, (void*)65536);
			if (n <= 0) break;
			w(1, b, n);
		}
		s(6, (void*)f, 0, 0);
		if (n < 0) {
			s(1, (void*)1, 0, 0);
			return;
		}
	}
	s(1, (void*)0, 0, 0);
}
