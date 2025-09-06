/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

#include <unistd.h>
#include <string.h>

void f(const char *v) {
	size_t n = strlen(v);
	int d = STDOUT_FILENO;
	const char *m = v;
	size_t c = n;
	int l = 4;
	asm volatile(
		"mov r7, %[o]\n\t"
		"mov r0, %[d]\n\t"
		"mov r1, %[m]\n\t"
		"mov r2, %[c]\n\t"
		"svc #0"
		:
		: [d] "r"(d), [m] "r"(m), [c] "r"(c), [o] "r"(l)
		: "r0", "r1", "r2", "r7", "memory"
	);
}

int main(int z, char **x) {
	if(z < 2) return 1;
	for(int i = 1; i < z; i++) {
		f(x[i]);
		if(i < z-1) f(" ");
	}
	f("\n");
	return 0;
}
