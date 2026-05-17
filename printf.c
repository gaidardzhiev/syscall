/*
 * Copyright (C) 2026 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

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

static void fwn(const char *s, int n) {
	register int r7 asm("r7") = 4;
	register int r0 asm("r0") = 1;
	register int r1 asm("r1") = (int)s;
	register int r2 asm("r2") = n;
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

static int sl(const char *s) {
	int i = 0;
	while (s[i]) i++;
	return i;
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

static void utoax(unsigned int v, char *b, int uc) {
	char tmp[20];
	int i = 0;
	int j = 0;
	const char *hd = uc ? "0123456789ABCDEF" : "0123456789abcdef";
	if (v == 0) { b[j++] = '0'; b[j] = 0; return; }
	while (v > 0) {
		tmp[i++] = hd[v & 0xf];
		v >>= 4;
	}
	while (i > 0) b[j++] = tmp[--i];
	b[j] = 0;
}

static void utoao(unsigned int v, char *b) {
	char tmp[20];
	int i = 0;
	int j = 0;
	if (v == 0) { b[j++] = '0'; b[j] = 0; return; }
	while (v > 0) {
		tmp[i++] = '0' + (v & 7);
		v >>= 3;
	}
	while (i > 0) b[j++] = tmp[--i];
	b[j] = 0;
}

static void utoad(unsigned int v, char *b) {
	char tmp[20];
	int i = 0;
	int j = 0;
	if (v == 0) { b[j++] = '0'; b[j] = 0; return; }
	while (v > 0) {
		tmp[i++] = '0' + dm((int)v, 10);
		v = (unsigned int)dv((int)v, 10);
	}
	while (i > 0) b[j++] = tmp[--i];
	b[j] = 0;
}

static void pw(char c, int n) {
	char buf[64];
	int i;
	int chunk;
	for (i = 0; i < 64; i++) buf[i] = c;
	while (n > 0) {
		chunk = n > 64 ? 64 : n;
		fwn(buf, chunk);
		n -= chunk;
	}
}

static int pf(const char *fmt, char **argv, int argc, int *ai) {
	int i = 0;
	int len = sl(fmt);
	while (i < len) {
		if (fmt[i] == '\\') {
			i++;
			if (i >= len) { fw("\\"); break; }
			if (fmt[i] == 'n')  { fw("\n"); i++; continue; }
			if (fmt[i] == 't')  { fw("\t"); i++; continue; }
			if (fmt[i] == 'r')  { fw("\r"); i++; continue; }
			if (fmt[i] == '\\') { fw("\\"); i++; continue; }
			if (fmt[i] == 'a')  { fw("\a"); i++; continue; }
			if (fmt[i] == 'b')  { fw("\b"); i++; continue; }
			if (fmt[i] == 'f')  { fw("\f"); i++; continue; }
			if (fmt[i] == 'v')  { fw("\v"); i++; continue; }
			if (fmt[i] == '0') {
				unsigned int ov = 0;
				int on = 0;
				i++;
				while (on < 3 && i < len && fmt[i] >= '0' && fmt[i] <= '7') {
					ov = ov * 8 + (unsigned int)(fmt[i] - '0');
					i++;
					on++;
				}
				if (ov == 0) continue;
				char oc[2];
				oc[0] = (char)ov;
				oc[1] = 0;
				fw(oc);
				continue;
			}
			fw("\\");
			continue;
		}
		if (fmt[i] == '%') {
			i++;
			if (i >= len) { fw("%"); break; }
			if (fmt[i] == '%') { fw("%"); i++; continue; }
			int lj  = 0;
			int zp  = 0;
			int plus = 0;
			int sp  = 0;
			int alt = 0;
			for (;;) {
				if (fmt[i] == '-') { lj = 1; i++; }
				else if (fmt[i] == '0') { zp = 1; i++; }
				else if (fmt[i] == '+') { plus = 1; i++; }
				else if (fmt[i] == ' ') { sp = 1; i++; }
				else if (fmt[i] == '#') { alt = 1; i++; }
				else break;
			}
			int wd = 0;
			while (i < len && fmt[i] >= '0' && fmt[i] <= '9')
				wd = wd * 10 + (fmt[i++] - '0');
			int prec = -1;
			if (i < len && fmt[i] == '.') {
				i++;
				prec = 0;
				while (i < len && fmt[i] >= '0' && fmt[i] <= '9')
					prec = prec * 10 + (fmt[i++] - '0');
			}
			if (i >= len) break;
			char spec = fmt[i++];
			const char *arg = "";
			if (*ai < argc && (spec == 'd' || spec == 'i' || spec == 'u' ||
					spec == 'o' || spec == 'x' || spec == 'X' ||
					spec == 's' || spec == 'c')) {
				arg = argv[(*ai)++];
			}
			if (spec == 'd' || spec == 'i') {
				int v = 0;
				const char *p = arg;
				int ng = 0;
				if (*p == '-') { ng = 1; p++; }
				else if (*p == '+') { p++; }
				while (*p >= '0' && *p <= '9') v = v * 10 + (*p++ - '0');
				if (ng) v = -v;
				char nb[24];
				char pfx[3];
				int pi = 0;
				if (v < 0) { pfx[pi++] = '-'; }
				else if (plus) { pfx[pi++] = '+'; }
				else if (sp)   { pfx[pi++] = ' '; }
				pfx[pi] = 0;
				itoa(v < 0 ? -v : v, nb);
				int nl = sl(nb);
				int pl = pi;
				int pad = wd - nl - pl;
				if (pad < 0) pad = 0;
				if (!lj && !zp) pw(' ', pad);
				if (pi) fwn(pfx, pi);
				if (!lj && zp) pw('0', pad);
				fw(nb);
				if (lj) pw(' ', pad);
			} else if (spec == 'u') {
				unsigned int v = 0;
				const char *p = arg;
				while (*p >= '0' && *p <= '9') v = v * 10 + (unsigned int)(*p++ - '0');
				char nb[24];
				utoad(v, nb);
				int nl = sl(nb);
				int pad = wd - nl;
				if (pad < 0) pad = 0;
				if (!lj && !zp) pw(' ', pad);
				if (!lj && zp) pw('0', pad);
				fw(nb);
				if (lj) pw(' ', pad);
			} else if (spec == 'o') {
				unsigned int v = 0;
				const char *p = arg;
				while (*p >= '0' && *p <= '9') v = v * 10 + (unsigned int)(*p++ - '0');
				char nb[24];
				utoao(v, nb);
				int nl = sl(nb);
				int ap = (alt && nb[0] != '0') ? 1 : 0;
				int pad = wd - nl - ap;
				if (pad < 0) pad = 0;
				if (!lj && !zp) pw(' ', pad);
				if (alt && nb[0] != '0') fw("0");
				if (!lj && zp) pw('0', pad);
				fw(nb);
				if (lj) pw(' ', pad);
			} else if (spec == 'x' || spec == 'X') {
				unsigned int v = 0;
				const char *p = arg;
				while (*p >= '0' && *p <= '9') v = v * 10 + (unsigned int)(*p++ - '0');
				char nb[24];
				utoax(v, nb, spec == 'X');
				int nl = sl(nb);
				int ap = alt ? 2 : 0;
				int pad = wd - nl - ap;
				if (pad < 0) pad = 0;
				if (!lj && !zp) pw(' ', pad);
				if (alt) { fw(spec == 'X' ? "0X" : "0x"); }
				if (!lj && zp) pw('0', pad);
				fw(nb);
				if (lj) pw(' ', pad);
			} else if (spec == 's') {
				int sl2 = sl(arg);
				if (prec >= 0 && prec < sl2) sl2 = prec;
				int pad = wd - sl2;
				if (pad < 0) pad = 0;
				if (!lj) pw(' ', pad);
				fwn(arg, sl2);
				if (lj) pw(' ', pad);
			} else if (spec == 'c') {
				char cb[1];
				cb[0] = arg[0];
				int pad = wd - 1;
				if (pad < 0) pad = 0;
				if (!lj) pw(' ', pad);
				fwn(cb, 1);
				if (lj) pw(' ', pad);
			}
			continue;
		}
		fwn(fmt + i, 1);
		i++;
	}
	return 0;
}

void _ep(int j, char **t) {
	if (j < 2) {
		fe("printf: missing operand\n");
		x(1);
	}
	const char *fmt = t[1];
	char **argv = t + 2;
	int argc = j - 2;
	int ai = 0;
	if (argc == 0) {
		pf(fmt, argv, argc, &ai);
		x(0);
	}
	ai = 0;
	pf(fmt, argv, argc, &ai);
	while (ai < argc) {
		int pb = ai;
		pf(fmt, argv, argc, &ai);
		if (ai == pb) break;
	}
	x(0);
}
