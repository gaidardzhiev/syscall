/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

static long fw(long nr, void *a0, void *a1, void *a2) {
	register long x8 asm("x8") = nr;
	register void *x0 asm("x0") = a0;
	register void *x1 asm("x1") = a1;
	register void *x2 asm("x2") = a2;
	long ret;
	asm volatile (
		"svc #0\n"
		: "=r" (x0), "=r" (ret)
		: "r" (x8), "r" (x0), "r" (x1), "r" (x2)
		: "memory"
	);
	ret = (long)x0;
	return ret;
}

static void fs(const char *s) {
	char b[256];
	int i;
	for (i = 0; i < 255 && s[i] != '\0'; i++)
		b[i] = s[i];
	b[i] = '\0';
	fw(64, (void*)1, b, (void*)(long)i);
}

static void delay(void) {
	volatile long k;
	for (k = 0; k < 4000000; k++)
		;
}

void _ep(void) {
	const char *m = "y";
	char nl = '\n';
	for (;;) {
		fs(m);
		fw(64, (void*)1, &nl, (void*)1);
		delay();
	}
}
