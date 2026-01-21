/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

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
		const char *d = "usage: ./wc <file>\n";
		register int r7 asm("r7") = 4;
		register int r0 asm("r0") = 2;
		register int r1 asm("r1") = (int)d;
		register int r2 asm("r2") = 20;
		asm volatile ("svc 0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
		register int r7x asm("r7") = 1;
		register int r0x asm("r0") = 1;
		asm volatile ("svc 0" : "=r" (r0x) : "r" (r7x), "r" (r0x) : "memory");
	}
	register int r7o asm("r7") = 5;
	register int r0o asm("r0") = (int)q[1];
	register int r1o asm("r1") = 0;
	asm volatile ("svc 0" : "=r" (r0o) : "r" (r7o), "r" (r0o), "r" (r1o) : "memory");
	if ((unsigned int)r0o > 1023) {
		register int r7x asm("r7") = 1;
		register int r0x asm("r0") = 1;
		asm volatile ("svc 0" : "=r" (r0x) : "r" (r7x), "r" (r0x) : "memory");
	}
	int f = r0o;
	int l = 0, w = 0, b = 0;
	int iw = 0;
	char cbuf[4096];
	register int r7r asm("r7") = 3;
	while (1) {
		register int r0r asm("r0") = f;
		register int r1r asm("r1") = (int)cbuf;
		register int r2r asm("r2") = 4096;
		asm volatile ("svc 0" : "=r" (r0r) : "r" (r7r), "r" (r0r), "r" (r1r), "r" (r2r) : "memory");
		int n = r0r;
		if (n <= 0) break;
		b += n;
		for (int i = 0; i < n; i++) {
			char c = cbuf[i];
			if (c == '\n') l++;
			int sp = (c == ' ' || c == '\n' || c == '\t' || c == '\r');
			if (sp) {
				iw = 0;
			} else if (!iw) {
				w++;
				iw = 1;
			}
		}
	}
	char ob[64];
	int p2 = 0;
	int n2 = l;
	int tp = 4;
	char t[6] = "     ";
	if (n2 == 0) t[tp--] = '0';
	while (n2 > 0) {
		t[tp--] = '0' + (n2 % 10);
		n2 /= 10;
	}
	for (int i = 0; i < 5; i++) ob[p2++] = t[i];
	ob[p2++] = ' ';
	n2 = w;
	tp = 4;
	t[5] = 0;
	t[0] = ' ';
	t[1] = ' ';
	t[2] = ' ';
	t[3] = ' ';
	t[4] = ' ';
	if (n2 == 0) t[tp--] = '0';
	while (n2 > 0) {
		t[tp--] = '0' + (n2 % 10);
		n2 /= 10;
	}
	for (int i = 0; i < 5; i++) ob[p2++] = t[i];
	ob[p2++] = ' ';
	n2 = b;
	tp = 4;
	t[0] = ' ';
	t[1] = ' ';
	t[2] = ' ';
	t[3] = ' ';
	t[4] = ' ';
	if (n2 == 0) t[tp--] = '0';
	while (n2 > 0) {
		t[tp--] = '0' + (n2 % 10);
		n2 /= 10;
	}
	for (int i = 0; i < 5; i++) ob[p2++] = t[i];
	ob[p2++] = ' ';
	for (int i = 0; q[1][i] && i < 20; i++) ob[p2++] = q[1][i];
	ob[p2++] = '\n';
	register int r7w asm("r7") = 4;
	register int r0w asm("r0") = 1;
	register int r1w asm("r1") = (int)ob;
	register int r2w asm("r2") = p2;
	asm volatile ("svc 0" : "=r" (r0w) : "r" (r7w), "r" (r0w), "r" (r1w), "r" (r2w) : "memory");
	register int r7c asm("r7") = 6;
	register int r0c asm("r0") = f;
	asm volatile ("svc 0" : "=r" (r0c) : "r" (r7c), "r" (r0c) : "memory");
	register int r7x asm("r7") = 1;
	register int r0x asm("r0") = 0;
	asm volatile ("svc 0" : "=r" (r0x) : "r" (r7x), "r" (r0x) : "memory");
}
