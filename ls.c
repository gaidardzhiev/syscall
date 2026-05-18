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

static int op(const char *path, int flags) {
	register int r7 asm("r7") = 5;
	register int r0 asm("r0") = (int)path;
	register int r1 asm("r1") = flags;
	register int r2 asm("r2") = 0;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
			: "memory");
	return r0;
}

static int cl(int fd) {
	register int r7 asm("r7") = 6;
	register int r0 asm("r0") = fd;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0)
			: "memory");
	return r0;
}

static int gd(int fd, char *buf, int cnt) {
	register int r7 asm("r7") = 217;
	register int r0 asm("r0") = fd;
	register int r1 asm("r1") = (int)buf;
	register int r2 asm("r2") = cnt;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2)
			: "memory");
	return r0;
}

static int st(const char *path, char *buf) {
	register int r7 asm("r7") = 195;
	register int r0 asm("r0") = (int)path;
	register int r1 asm("r1") = (int)buf;
	asm volatile ("swi #0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1)
			: "memory");
	return r0;
}

static int sl(const char *s) {
	int i = 0;
	while (s[i]) i++;
	return i;
}

static int q(const char *a, const char *b) {
	while (*a && *b && *a == *b) { a++; b++; }
	return *a == *b;
}

static void sc(char *d, const char *s) {
	while (*s) *d++ = *s++;
	*d = 0;
}

static int dv(int a, int b) {
	int neg = 0;
	unsigned int ua, ub, qu, bit;
	if (a < 0) { neg = !neg; ua = (unsigned int)-a; } else { ua = (unsigned int)a; }
	if (b < 0) { neg = !neg; ub = (unsigned int)-b; } else { ub = (unsigned int)b; }
	qu = 0;
	bit = 1;
	while (ub < ua && !(ub & 0x80000000u)) { ub <<= 1; bit <<= 1; }
	while (bit) {
		if (ua >= ub) { ua -= ub; qu |= bit; }
		ub >>= 1;
		bit >>= 1;
	}
	return neg ? -(int)qu : (int)qu;
}

static int dm(int a, int b) {
	return a - dv(a, b) * b;
}

static void itoa(unsigned int v, char *b) {
	char tmp[20];
	int i = 0;
	int j = 0;
	if (v == 0) { b[j++] = '0'; b[j] = 0; return; }
	while (v > 0) { tmp[i++] = '0' + dm((int)v, 10); v = (unsigned int)dv((int)v, 10); }
	while (i > 0) b[j++] = tmp[--i];
	b[j] = 0;
}

struct di {
	unsigned int in0;
	unsigned int in1;
	unsigned int of0;
	unsigned int of1;
	unsigned short rl;
	unsigned char tp;
	char nm[256];
};

#define DT_DIR  4
#define DT_LNK  10
#define ST_SIZE_OFF 48
#define ST_MODE_OFF 16
#define ST_UID_OFF 24
#define ST_GID_OFF 28
#define ST_NLINK_OFF 20
#define S_IFMT 0170000
#define S_IFREG 0100000
#define S_IFDIR 0040000
#define S_IFLNK 0120000
#define S_IFIFO 0010000
#define S_IFBLK 0060000
#define S_IFCHR 0020000
#define S_IFSOCK 0140000
#define BUF 4096
#define MAXE 2048

static char eb[MAXE][256];
static int  ec;

static void es(int n) {
	int i, j;
	char tmp[256];
	for (i = 1; i < n; i++) {
		sc(tmp, eb[i]);
		j = i - 1;
		while (j >= 0 && eb[j][0] > tmp[0]) {
			sc(eb[j + 1], eb[j]);
			j--;
		}
		sc(eb[j + 1], tmp);
	}
}

static void esf(int n) {
	int i, j;
	char tmp[256];
	for (i = 1; i < n; i++) {
		sc(tmp, eb[i]);
		j = i - 1;
		while (j >= 0) {
			const char *a = eb[j];
			const char *b = tmp;
			int gt = 0;
			while (*a && *b) {
				if (*a > *b) { gt = 1; break; }
				if (*a < *b) { break; }
				a++; b++;
			}
			if (!gt && *a == 0 && *b != 0) gt = 0;
			if (!gt && *a != 0 && *b == 0) gt = 1;
			if (!gt) break;
			sc(eb[j + 1], eb[j]);
			j--;
		}
		sc(eb[j + 1], tmp);
	}
}

static void mtp(unsigned int mode, char *out) {
	unsigned int ft = mode & S_IFMT;
	if (ft == S_IFREG)  out[0] = '-';
	else if (ft == S_IFDIR) out[0] = 'd';
	else if (ft == S_IFLNK) out[0] = 'l';
	else if (ft == S_IFIFO) out[0] = 'p';
	else if (ft == S_IFBLK) out[0] = 'b';
	else if (ft == S_IFCHR) out[0] = 'c';
	else if (ft == S_IFSOCK) out[0] = 's';
	else out[0] = '?';
	out[1] = (mode & 0400) ? 'r' : '-';
	out[2] = (mode & 0200) ? 'w' : '-';
	out[3] = (mode & 04000) ? 's' : ((mode & 0100) ? 'x' : '-');
	out[4] = (mode & 040) ? 'r' : '-';
	out[5] = (mode & 020) ? 'w' : '-';
	out[6] = (mode & 02000) ? 's' : ((mode & 010) ? 'x' : '-');
	out[7] = (mode & 04) ? 'r' : '-';
	out[8] = (mode & 02) ? 'w' : '-';
	out[9] = (mode & 01000) ? 't' : ((mode & 01) ? 'x' : '-');
	out[10] = 0;
}

static int pw(char *buf, int w, const char *s, int rj) {
	int len = sl(s);
	int pad = w - len;
	int i = 0;
	if (rj) {
		while (pad-- > 0) buf[i++] = ' ';
		while (*s) buf[i++] = *s++;
	} else {
		while (*s) buf[i++] = *s++;
		while (pad-- > 0) buf[i++] = ' ';
	}
	return i;
}

static int ls(const char *path, int fa, int fl, int fd2) {
	char dbuf[BUF];
	int fd;
	int nr;
	int i;
	char statbuf[104];
	char pb[512];
	char nb[24];
	fd = op(path, 0x10000 | 0200000);
	if (fd < 0) {
		fe("ls: cannot open directory\n");
		return -1;
	}
	ec = 0;
	for (;;) {
		nr = gd(fd, dbuf, BUF);
		if (nr <= 0) break;
		i = 0;
		while (i < nr) {
			struct di *d = (struct di *)(dbuf + i);
			if (!fa && d->nm[0] == '.') { i += d->rl; continue; }
			if (ec < MAXE) {
				sc(eb[ec], d->nm);
				ec++;
			}
			i += d->rl;
		}
	}
	cl(fd);
	esf(ec);
	if (!fl) {
		for (i = 0; i < ec; i++) {
			fw(eb[i]);
			fw("\n");
		}
		return 0;
	}
	for (i = 0; i < ec; i++) {
		sc(pb, path);
		if (!q(path, "/")) {
			int pl = sl(pb);
			pb[pl] = '/';
			pb[pl + 1] = 0;
		}
		sc(pb + sl(pb), eb[i]);
		int sr = st(pb, statbuf);
		if (sr < 0) continue;
		unsigned int mode = *(unsigned int *)(statbuf + ST_MODE_OFF);
		unsigned int nlink = *(unsigned int *)(statbuf + ST_NLINK_OFF);
		unsigned int uid = *(unsigned int *)(statbuf + ST_UID_OFF);
		unsigned int gid = *(unsigned int *)(statbuf + ST_GID_OFF);
		unsigned int size = *(unsigned int *)(statbuf + ST_SIZE_OFF);
		char mp[12];
		mtp(mode, mp);
		char line[512];
		int li = 0;
		line[li++] = mp[0]; line[li++] = mp[1]; line[li++] = mp[2];
		line[li++] = mp[3]; line[li++] = mp[4]; line[li++] = mp[5];
		line[li++] = mp[6]; line[li++] = mp[7]; line[li++] = mp[8];
		line[li++] = mp[9]; line[li++] = ' ';
		itoa(nlink, nb);
		li += pw(line + li, 4, nb, 1);
		line[li++] = ' ';
		itoa(uid, nb);
		li += pw(line + li, 5, nb, 1);
		line[li++] = ' ';
		itoa(gid, nb);
		li += pw(line + li, 5, nb, 1);
		line[li++] = ' ';
		itoa(size, nb);
		li += pw(line + li, 8, nb, 1);
		line[li++] = ' ';
		sc(line + li, eb[i]);
		li += sl(eb[i]);
		line[li++] = '\n';
		line[li]   = 0;
		register int r7w asm("r7") = 4;
		register int r0w asm("r0") = fd2;
		register int r1w asm("r1") = (int)line;
		register int r2w asm("r2") = li;
		asm volatile ("swi #0"
				: "=r"(r0w)
				: "r"(r7w), "r"(r0w), "r"(r1w), "r"(r2w)
				: "memory");
	}
	return 0;
}

void _ep(int j, char **t) {
	int fa = 0;
	int fl = 0;
	int i;
	int np = 0;
	char *pa[64];
	for (i = 1; i < j; i++) {
		if (t[i][0] == '-' && t[i][1] != 0) {
			const char *p = t[i] + 1;
			while (*p) {
				if (*p == 'a') fa = 1;
				else if (*p == 'l') fl = 1;
				p++;
			}
		} else {
			if (np < 64) pa[np++] = t[i];
		}
	}
	if (np == 0) {
		pa[0] = ".";
		np = 1;
	}
	for (i = 0; i < np; i++) {
		if (np > 1) { fw(pa[i]); fw(":\n"); }
		if (ls(pa[i], fa, fl, 1) < 0) x(1);
	}
	x(0);
}
