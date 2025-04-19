#include <stdint.h>

struct T {
	uint32_t a;
	uint32_t b;
};

int c(const struct T *d, struct T *e) {
	register const struct T *f __asm__("r0") = d;
	register struct T *g __asm__("r1") = e;
	register int h __asm__("r7") = 162;
	__asm__ volatile (
		"svc 0\n"
		: "+r"(f)
		: "r"(g), "r"(h)
		: "memory"
	);
	return (int)f;
}

void i(unsigned int j) {
	struct T k = { j, 0 };
	c(&k, 0);
}

unsigned int l(const char *m) {
	unsigned int n = 0;
	while (*m) {
		char o = *m++;
		if (o < '0' || o > '9') {
			break;
		}
		n = n * 10 + (o - '0');
	}
	return n;
}

void p(const char *q) {
	const char *r = q;
	int s = 0;
	while (r[s]) s++;
	register int t __asm__("r0") = 2;
	register const char *u __asm__("r1") = q;
	register int v __asm__("r2") = s;
	register int w __asm__("r7") = 4;
	__asm__ volatile (
		"svc 0\n"
		:
		: "r"(t), "r"(u), "r"(v), "r"(w)
		: "memory"
	);
}

int main(int x, char **y) {
	if (x < 2) {
		p("usage: ./sleep <sec>\n");
		return 1;
	}
	unsigned int z = l(y[1]);
	if (z == 0) {
		p("invalid time interval...\n");
		return 2;
	}
	i(z);
	return 0;
}
