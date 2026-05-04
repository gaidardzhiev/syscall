/*
 * Copyright (C) 2026 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only                                      */

static void fw(const char *s) {
	int i = 0;
	while (s[i]) i++;
	register int r7 asm("r7") = 4;
	register int r0 asm("r0") = 1;
	register int r1 asm("r1") = (int)s;
	register int r2 asm("r2") = i;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
			: "memory");
}

static void fe(const char *s) {
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

static int ni(const char *s) {
	const char *p = s;
	if (*p == '-') p++;
	if (!(*p >= '0' && *p <= '9')) return 0;
	while (*p >= '0' && *p <= '9') p++;
	return *p == 0;
}

static int n(const char *s) {
	int r = 0;
	int g = 0;
	if (*s == '-') { g = 1; s++; }
	while (*s >= '0' && *s <= '9')
		r = r * 10 + (*s++ - '0');
	return g ? -r : r;
}

static int dv(int a, int b) {
	int neg = 0;
	unsigned int ua, ub, q, bit;
	if (a < 0) { neg = !neg; ua = (unsigned int)-a; } else { ua = (unsigned int)a; }
	if (b < 0) { neg = !neg; ub = (unsigned int)-b; } else { ub = (unsigned int)b; }
	q = 0;
	bit = 1;
	while (ub < ua && !(ub & 0x80000000u)) { ub <<= 1; bit <<= 1; }
	while (bit) {
		if (ua >= ub) { ua -= ub; q |= bit; }
		ub >>= 1;
		bit >>= 1;
	}
	return neg ? -(int)q : (int)q;
}

static int dm(int a, int b) {
	return a - dv(a, b) * b;
}

static char ob[24];

static void itoa(int v, char *b) {
	char tmp[20];
	int i = 0;
	int j = 0;
	int neg = 0;
	if (v < 0) { neg = 1; v = -v; }
	if (v == 0) { b[j++] = '0'; b[j] = 0; return; }
	while (v > 0) { tmp[i++] = '0' + dm(v, 10); v = dv(v, 10); }
	if (neg) tmp[i++] = '-';
	while (i > 0) b[j++] = tmp[--i];
	b[j] = 0;
}

static int gp;
static int gc;
static char **gv;

static const char *tok(void) {
	if (gp >= gc) return 0;
	return gv[gp];
}

static const char *adv(void) {
	if (gp >= gc) return 0;
	return gv[gp++];
}

static int ep(void);

static int sc(const char *s, int p, int l) {
	int len = sl(s);
	int i;
	char out[256];
	int oi = 0;
	if (p < 1) p = 1;
	if (p > len + 1) p = len + 1;
	int st = p - 1;
	int en = st + l;
	if (en > len) en = len;
	if (st >= en) return 0;
	for (i = st; i < en && oi < 255; i++)
		out[oi++] = s[i];
	out[oi] = 0;
	fw(out);
	return oi;
}

static int atom(void) {
	const char *t = tok();
	if (!t) {
		fe("expr: missing operand\n");
		x(2);
	}
	if (q(t, "(")) {
		adv();
		int v = ep();
		if (!tok() || !q(tok(), ")")) {
			fe("expr: syntax error\n");
			x(2);
		}
		adv();
		return v;
	}
	if (q(t, "length")) {
		adv();
		const char *s = tok();
		if (!s) { fe("expr: missing operand\n"); x(2); }
		adv();
		return sl(s);
	}
	if (q(t, "substr")) {
		adv();
		const char *s = tok();
		if (!s) { fe("expr: missing operand\n"); x(2); }
		adv();
		const char *ps = tok();
		if (!ps) { fe("expr: missing operand\n"); x(2); }
		adv();
		const char *ls = tok();
		if (!ls) { fe("expr: missing operand\n"); x(2); }
		adv();
		int r = sc(s, n(ps), n(ls));
		fw("\n");
		x(r ? 0 : 1);
	}
	if (q(t, "index")) {
		adv();
		const char *s = tok();
		if (!s) { fe("expr: missing operand\n"); x(2); }
		adv();
		const char *cs = tok();
		if (!cs) { fe("expr: missing operand\n"); x(2); }
		adv();
		int i, j;
		int ls = sl(s);
		int lc = sl(cs);
		for (i = 0; i < ls; i++)
			for (j = 0; j < lc; j++)
				if (s[i] == cs[j]) {
					itoa(i + 1, ob);
					fw(ob);
					fw("\n");
					x(0);
				}
		fw("0\n");
		x(1);
	}
	adv();
	if (!ni(t)) {
		fe("expr: non-integer argument\n");
		x(2);
	}
	return n(t);
}

static int md(void) {
	int v = atom();
	for (;;) {
		const char *t = tok();
		if (!t) break;
		if (q(t, "*")) {
			adv(); v = v * atom();
		} else if (q(t, "/")) {
			adv();
			int d = atom();
			if (d == 0) { fe("expr: division by zero\n"); x(2); }
			v = dv(v, d);
		} else if (q(t, "%")) {
			adv();
			int d = atom();
			if (d == 0) { fe("expr: division by zero\n"); x(2); }
			v = dm(v, d);
		} else break;
	}
	return v;
}

static int as(void) {
	int v = md();
	for (;;) {
		const char *t = tok();
		if (!t) break;
		if      (q(t, "+")) { adv(); v = v + md(); }
		else if (q(t, "-")) { adv(); v = v - md(); }
		else break;
	}
	return v;
}

static int cm(void) {
	int a = as();
	const char *t = tok();
	if (!t) return a;
	if (q(t, "="))  { adv(); int b = as(); return a == b; }
	if (q(t, "!=")) { adv(); int b = as(); return a != b; }
	if (q(t, "<"))  { adv(); int b = as(); return a <  b; }
	if (q(t, "<=")) { adv(); int b = as(); return a <= b; }
	if (q(t, ">"))  { adv(); int b = as(); return a >  b; }
	if (q(t, ">=")) { adv(); int b = as(); return a >= b; }
	return a;
}

static int ea(void) {
	int a = cm();
	const char *t = tok();
	if (!t) return a;
	if (q(t, "&")) {
		adv();
		int b = cm();
		if (a == 0 || b == 0) return 0;
		return a;
	}
	return a;
}

static int ep(void) {
	int a = ea();
	const char *t = tok();
	if (!t) return a;
	if (q(t, "|")) {
		adv();
		int b = ea();
		return a != 0 ? a : b;
	}
	return a;
}

void _ep(int j, char **t) {
	if (j < 2) {
		fe("expr: missing operand\n");
		x(2);
	}
	gc = j - 1;
	gv = t + 1;
	gp = 0;
	int v = ep();
	if (gp < gc) {
		fe("expr: syntax error\n");
		x(2);
	}
	itoa(v, ob);
	fw(ob);
	fw("\n");
	x(v != 0 ? 0 : 1);
}
