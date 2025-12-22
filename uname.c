/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

struct uts {
	char s[65];
	char n[65];
	char r[65];
	char v[65];
	char m[65];
	char d[65];
};

void fun(int nr, void *z) {
	register int r7 asm("r7") = nr;
	register void *r0 asm("r0") = z;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0) : "memory");
}

void fw(int nr, void *z, void *x, int y) {
	register int r7 asm("r7") = nr;
	register void *r0 asm("r0") = z;
	register void *r1 asm("r1") = x;
	register int r2 asm("r2") = y;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
}

void fpnul(const char *k, int l) {
	char b[66];
	int i;
	for (i = 0; i < l && k[i] != '\0'; i++) b[i] = k[i];
	b[i] = '\0';
	fw(4, (void*)1, b, i);
	fw(4, (void*)1, (void*)" ", 1);
}

void fpn() {
	char nl = '\n';
	fw(4, (void*)1, &nl, 1);
}

void _ep() {
	struct uts q;
	fun(122, &q);
	fpnul(q.s,65);
	fpnul(q.n,65);
	fpnul(q.r,65);
	fpnul(q.v,65);
	fpnul(q.m,65);
	fpnul(q.d,65);
	fpn();
	fun(1, 0);
}
