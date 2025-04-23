#include <stddef.h>
#include <stdint.h>

static inline int z(int y, int x, int w, int v) {
	int u;
	asm volatile (
		"mov r7, %1\n"
		"mov r0, %2\n"
		"mov r1, %3\n"
		"mov r2, %4\n"
		"swi 0\n"
		"mov %0, r0\n"
		: "=r"(u)
		: "r"(y), "r"(x), "r"(w), "r"(v)
		: "r0", "r1", "r2", "r7"
	);
	return u;
}

static inline int o(int n, int m) {
	int l;
	asm volatile (
		"mov r7, %1\n"
		"mov r0, %2\n"
		"swi 0\n"
		"mov %0, r0\n"
		: "=r"(l)
		: "r"(n), "r"(m)
		: "r0", "r7"
	);
	return l;
}

void k(const char *j, size_t i) {
	z(4, 1, (int)j, i);
}

void h(int g) {
	o(1, g);
	__builtin_unreachable();
}

int main() {
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
	return 0;
}
