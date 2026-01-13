/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

void fw(int nr, void *z, void *x, int y) {
	register int r7 asm("r7") = nr;
	register void *r0 asm("r0") = z;
	register void *r1 asm("r1") = x;
	register int r2 asm("r2") = y;
	asm volatile ("svc #0" : "=r" (r0) : "r" (r7), "r" (r0), "r" (r1), "r" (r2) : "memory");
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
	if (p < 2) {
		fw(1, (void*)0, 0, 0);
		return;
	}
	int i;
	for (i = 1; i < p; i++) {
		const char *l = q[i];
		char b[256];
		int j = 0, k = 0;
		while (j < 255 && l[j] != '\0') {
			if (l[j] == '\\' && l[j+1] == 'n') {
				b[k++] = '\n';
				j += 2;
			} else {
				b[k++] = l[j++];
			}
		}
		fw(4, (void*)1, b, k);
		if (i < p - 1) {
			fw(4, (void*)1, " ", 1);
		}
	}
	fw(4, (void*)1, "\n", 1);
	fw(1, (void*)0, 0, 0);
}
