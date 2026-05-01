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

static int sl(const char *s) {
	int i = 0;
	while (s[i]) i++;
	return i;
}

static int n(const char *s) {
	int r = 0;
	int g = 0;
	if (*s == '-') { g = 1; s++; }
	while (*s >= '0' && *s <= '9')
		r = r * 10 + (*s++ - '0');
	return g ? -r : r;
}

static unsigned int u32(const char *p) {
	const unsigned char *b = (const unsigned char *)p;
	return ((unsigned int)b[0]) |
	       ((unsigned int)b[1] << 8) |
	       ((unsigned int)b[2] << 16) |
	       ((unsigned int)b[3] << 24);
}

static long long s64(const char *p) {
	unsigned long long lo = u32(p);
	unsigned long long hi = u32(p + 4);
	return (long long)(lo | (hi << 32));
}

static int ismode(unsigned int m) {
	unsigned int t = m & 0170000;
	return t == 0100000 || t == 0040000 || t == 0120000 ||
	       t == 0060000 || t == 0020000 || t == 0010000 ||
	       t == 0140000;
}

static int sys_access(const char *p, int m) {
	register int r7 asm("r7") = 33;
	register int r0 asm("r0") = (int)p;
	register int r1 asm("r1") = m;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1)
			: "memory");
	return r0;
}

static int sys_open(const char *p, int f, int m) {
	register int r7 asm("r7") = 5;
	register int r0 asm("r0") = (int)p;
	register int r1 asm("r1") = f;
	register int r2 asm("r2") = m;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
			: "memory");
	return r0;
}

static int sys_close(int fd) {
	register int r7 asm("r7") = 6;
	register int r0 asm("r0") = fd;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0)
			: "memory");
	return r0;
}

static int sys_lseek(int fd, int off, int wh) {
	register int r7 asm("r7") = 19;
	register int r0 asm("r0") = fd;
	register int r1 asm("r1") = off;
	register int r2 asm("r2") = wh;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
			: "memory");
	return r0;
}

static int fsize_pos(const char *p) {
	int fd = sys_open(p, 0, 0);
	int s;
	if (fd < 0) return 0;
	s = sys_lseek(fd, 0, 2);
	sys_close(fd);
	return s > 0;
}

static int fs(const char *p, unsigned int *mode, long long *size) {
	char b[128];
	unsigned int m;
	int i;
	register int r7 asm("r7") = 195;
	register int r0 asm("r0") = (int)p;
	register int r1 asm("r1") = (int)b;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1)
			: "memory");
	if (r0 != 0) return r0;
	if (mode) {
		m = u32(b + 16);
		if (!ismode(m)) {
			m = 0;
			for (i = 0; i <= 124; i += 4) {
				unsigned int z = u32(b + i);
				if (ismode(z)) { m = z; break; }
			}
		}
		*mode = m;
	}
	if (size) *size = s64(b + 44);
	return 0;
}

static int e(int c, char **v) {
	if (c == 0) return 0;
	if (c == 1) return sl(v[0]) > 0;
	if (c == 2 && v[0][0] == '-' && v[0][2] == 0) {
		unsigned int m = 0;
		long long sz = 0;
		char f = v[0][1];
		if (f == 'z') return sl(v[1]) == 0;
		else if (f == 'n') return sl(v[1]) > 0;
		else if (f == 'e') return fs(v[1], 0, 0) == 0;
		else if (f == 'f') return fs(v[1], &m, 0) == 0 && (m & 0170000) == 0100000;
		else if (f == 'd') return fs(v[1], &m, 0) == 0 && (m & 0170000) == 0040000;
		else if (f == 's') return fs(v[1], 0, &sz) == 0 && (sz > 0 || fsize_pos(v[1]));
		else if (f == 'r') return sys_access(v[1], 4) == 0;
		else if (f == 'w') return sys_access(v[1], 2) == 0;
		else if (f == 'x') return sys_access(v[1], 1) == 0;
	}
	if (c == 3) {
		if (q(v[1], "="))  return  q(v[0], v[2]);
		if (q(v[1], "!=")) return !q(v[0], v[2]);
	}
	if (c == 3 && v[1][0] == '-') {
		int a = n(v[0]);
		int b = n(v[2]);
		if (q(v[1], "-eq")) return a == b;
		if (q(v[1], "-ne")) return a != b;
		if (q(v[1], "-lt")) return a <  b;
		if (q(v[1], "-le")) return a <= b;
		if (q(v[1], "-gt")) return a >  b;
		if (q(v[1], "-ge")) return a >= b;
	}
	fw("test: invalid expression\n");
	x(2);
	return 0;
}

static int ep(int c, char **v);

static int en(int c, char **v) {
	if (c > 0 && q(v[0], "!")) return !en(c - 1, v + 1);
	return e(c, v);
}

static int ea(int c, char **v) {
	int i;
	for (i = 0; i < c; i++) {
		if (q(v[i], "-a")) return ea(i, v) && en(c - i - 1, v + i + 1);
	}
	return en(c, v);
}

static int ep(int c, char **v) {
	int i;
	for (i = 0; i < c; i++) {
		if (q(v[i], "-o")) return ep(i, v) || ea(c - i - 1, v + i + 1);
	}
	return ea(c, v);
}

static int ev(int c, char **v) {
	return ep(c, v);
}

void _ep(int j, char **t) {
	int c = j - 1;
	char **v = t + 1;
	int l = sl(t[0]);
	if (c == 0) x(1);
	if (l > 0 && t[0][l - 1] == '[') {
		if (!q(v[c - 1], "]")) {
			fw("test: missing ]\n");
			x(2);
		}
		c--;
		if (c == 0) x(1);
	}
	x(ev(c, v) ? 0 : 1);
}
