/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

#define I 0
#define O 1

void _ep() {
	char b[512];
	char *x[64];
	char pb[256];
	int l;
	while (1) {
		asm volatile (
			"mov r0, %0\n"
			"mov r1, %1\n"
			"mov r2, %2\n"
			"mov r7, %3\n"
			"svc 0\n"
			:
			: "r"(O), "r"("root# "), "r"(6), "r"(4)
			: "r0", "r1", "r2", "r7"
		);
		asm volatile (
			"mov r0, %1\n"
			"mov r1, %2\n"
			"mov r2, %3\n"
			"mov r7, %4\n"
			"svc 0\n"
			"mov %0, r0\n"
			: "=r"(l)
			: "r"(I), "r"(b), "r"(255), "r"(3)
			: "r0", "r1", "r2", "r7"
		);
		if (l <= 0) break;
		b[l - 1] = 0;
		if (b[0] == 'e' && b[1] == 'x' && b[2] == 'i' && b[3] == 't' && (b[4] == 0 || b[4] == ' ')) {
			break;
		}
		int z = 0;
		char *p = b;
		while (*p) {
			while (*p == ' ') p++;
			if (!*p) break;
			x[z++] = p;
			while (*p && *p != ' ') p++;
			if (*p) *p++ = 0;
			if (z >= 31) break;
		}
		x[z] = 0;
		if (z == 0) continue;
		if (z > 0 && x[0][0] == 'c' && x[0][1] == 'd' && x[0][2] == 0) {
			if (z > 1) {
				asm volatile (
					"mov r0, %0\n"
					"mov r7, #12\n"
					"svc 0\n"
					:
					: "r"(x[1])
					: "r0", "r7", "memory"
				);
			}
			continue;
		}
		char *m = x[0];
		if (m[0] != '/') {
			const char *e[] = {"/bin/", "/usr/bin/"};
			int f = 0;
			for (int i = 0; i < 2; i++) {
				char *d = pb;
				const char *y = e[i];
				while (*y) *d++ = *y++;
				const char *c = m;
				while (*c) *d++ = *c++;
				*d = 0;
				x[0] = pb;
				m = pb;
				f = 1;
				break;
			}
			if (!f) continue;
		}
		int t;
		asm volatile (
			"mov r7, %1\n"
			"svc 0\n"
			"mov %0, r0\n"
			: "=r"(t)
			: "r"(2)
			: "r0", "r7"
		);
		if (t == 0) {
			asm volatile (
				"mov r0, %0\n"
				"mov r1, %1\n"
				"mov r2, #0\n"
				"mov r7, %2\n"
				"svc 0\n"
				:
				: "r"(m), "r"(x), "r"(11)
				: "r0", "r1", "r2", "r7"
			);
			asm volatile (
				"mov r0, #1\n"
				"mov r7, %0\n"
				"svc 0\n"
				:
				: "r"(1)
				: "r0", "r7"
			);
		} else {
			int w;
			asm volatile (
				"mov r0, %1\n"
				"mov r1, %2\n"
				"mov r2, #0\n"
				"mov r3, #0\n"
				"mov r7, %3\n"
				"svc 0\n"
				"mov %0, r0\n"
				: "=r"(w)
				: "r"(t), "r"(&w), "r"(114)
				: "r0", "r1", "r2", "r3", "r7"
			);
		}
	}
	asm volatile (
		"mov r0, #0\n"
		"mov r7, %0\n"
		"svc 0\n"
		:
		: "r"(1)
		: "r0", "r7"
	);
}
