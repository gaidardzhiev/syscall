#define I 0
#define O 1

void _start() {
	char b[512];
	char *x[64];
	int l;
	while (1) {
		asm volatile (
			"mov r0, %0\n"
			"mov r1, %1\n"
			"mov r2, %2\n"
			"mov r7, %3\n"
			"svc 0\n"
			:
			: "r"(O), "r"(" sh # "), "r"(6), "r"(4)
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
				: "r"(x[0]), "r"(x), "r"(11)
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
