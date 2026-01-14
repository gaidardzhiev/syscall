/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

int z(int y, int x, int w, int v) {
	register int r7 asm("r7") = y;
	register int r0 asm("r0") = x;
	register int r1 asm("r1") = w;
	register int r2 asm("r2") = v;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
	return r0;
}

int o(int n, int m) {
	register int r7 asm("r7") = n;
	register int r0 asm("r0") = m;
	asm volatile ("swi #0" : "=r" (r0) : "r" (r7), "r" (r0) : "memory");
	return r0;
}

void k(const char *j, int i) {
	z(4, 1, (int)j, i);
}

void h(int g) {
	o(1, g);
	__builtin_unreachable();
}

void _ep() {
	char f[512];
	int e = z(54, 0, 0x5401, (int)f);
	if (e < 0) {
		const char *d = "not a tty...\n";
		k(d, 10);
		h(1);
	}
	const char *c = "/proc/self/fd/0";
	int b = z(85, (int)c, (int)f, 512-1);
	if (b < 0) {
		const char *a = "unknown...\n";
		k(a, 8);
		h(1);
	}
	f[b] = '\n';
	k(f, b+1);
	h(0);
}
