/*
 * Copyright (C) 2026 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

static void fw(const char *s) {
	int i = 0;
	while (s[i]) i++;
	register int r7 asm("r7") = 4;
	register int r0 asm("r0") = 2;
	register int r1 asm("r1") = (int)s;
	register int r2 asm("r2") = i;
	asm volatile ("swi #0"
		: "=r"(r0)
		: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
		: "memory");
}

static void x(int c) {
	register int r7 asm("r7") = 1;
	register int r0 asm("r0") = c;
	asm volatile ("swi #0"
		: "=r"(r0)
		: "r"(r7), "r"(r0)
		: "memory");
	for (;;);
}

static int q(const char *a, const char *b) {
	while (*a && *b && *a == *b) { a++; b++; }
	return *a == *b;
}

static int n(const char *s) {
	int r = 0;
	int base = 8;
	if (s[0] == '0' && s[1] == 'x') { base = 16; s += 2; }
	else if (s[0] == '0') s++;
	while (*s) {
		int d;
		if (*s >= '0' && *s <= '9') d = *s - '0';
		else if (*s >= 'a' && *s <= 'f') d = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'F') d = *s - 'A' + 10;
		else break;
		r = r * base + d;
		s++;
	}
	return r;
}

static int mk(const char *p, int m) {
	register int r7 asm("r7") = 39;
	register int r0 asm("r0") = (int)p;
	register int r1 asm("r1") = m;
	asm volatile ("swi #0"
		: "=r"(r0)
		: "r"(r7), "r"(r0), "r"(r1)
		: "memory");
	return r0;
}

void _ep(int j, char **t) {
	int i = 1;
	int m = 0755;
	if (j < 2) {
		fw("mkdir: missing operand\n");
		x(1);
	}
	if (q(t[1], "-m")) {
		if (j < 4) {
			fw("mkdir: missing operand\n");
			x(1);
		}
		m = n(t[2]);
		i = 3;
	}
	for (; i < j; i++) {
		int r = mk(t[i], m);
		if (r < 0) {
			fw("mkdir: cannot create directory '");
			fw(t[i]);
			fw("'\n");
			x(1);
		}
	}
	x(0);
}
