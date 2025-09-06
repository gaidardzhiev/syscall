/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

#include <unistd.h>
#include <sys/syscall.h>
#include <linux/fcntl.h>

static int w(int f, const char *b, int c) {
	int t = 0;
	while (t < c) {
		int r = syscall(4, f, b + t, c - t);
		if (r <= 0) return r;
		t += r;
	}
	return t;
}

int main(int a, char *v[]) {
	int f;
	char b[65536];
	int n;
	if (a < 2) {
		f = 0;
		while ((n = syscall(3, f, b, 65536)) > 0) {
			if (w(1, b, n) < 0)
				syscall(1, 1);
		}
		if (n < 0)
			syscall(1, 1);
		syscall(1, 0);
	}
	for (int i = 1; i < a; i++) {
		f = syscall(5, v[i], O_RDONLY, 0);
		if (f < 0) {
			const char *z = "no such file:";
			const char *x = "\n";
			syscall(4, 2, z, 32);
			const char *fn = v[i];
			int l = 0;
			while (fn[l]) l++;
			syscall(4, 2, fn, l);
			syscall(4, 2, x, 1);
			continue;
		}
		while ((n = syscall(3, f, b, 65536)) > 0) {
			if (w(1, b, n) < 0) {
				syscall(6, f);
				syscall(1, 1);
			}
		}
		syscall(6, f);
		if (n < 0)
			syscall(1, 1);
	}
	syscall(1, 0);
	return 0;
}
