/*
 * Copyright (C) 2026 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

static inline int o(int nr, int a, int b) {
	register int r7 asm("r7") = nr;
	register int r0 asm("r0") = a;
	register int r1 asm("r1") = b;
	asm volatile ("swi #0"
		      : "=r"(r0)
		      : "r"(r7), "r"(r0), "r"(r1)
		      : "memory");
	return r0;
}

static inline int sc(int nr, int a) {
	register int r7 asm("r7") = nr;
	register int r0 asm("r0") = a;
	asm volatile ("swi #0"
		      : "=r"(r0)
		      : "r"(r7), "r"(r0)
		      : "memory");
	return r0;
}

static int sl(const char *s) {
	int i = 0;
	while (s[i]) i++;
	return i;
}

static void fw(int fd, const char *s) {
	register int r7 asm("r7") = 4;
	register int r0 asm("r0") = fd;
	register int r1 asm("r1") = (int)s;
	register int r2 asm("r2") = sl(s);
	asm volatile ("swi #0"
		      : "=r"(r0)
		      : "r"(r7), "r"(r0), "r"(r1), "r"(r2)
		      : "memory");
}

static int j(const char *s) {
	int n = 0;
	int g = 0;
	if (*s == '-') {
		g = 1;
		s++;
	}
	while (*s >= '0' && *s <= '9')
		n = n * 10 + (*s++ - '0');
	return g ? -n : n;
}

static int se(const char *a, const char *b) {
	while (*a && *b && *a == *b) {
		a++;
		b++;
	}
	return *a == *b;
}

static const struct {
	const char *n;
	int s;
} t[] = {
	{"HUP",1},
	{"SIGHUP",1},
	{"INT",2},
	{"SIGINT",2},
	{"QUIT",3},
	{"SIGQUIT",3},
	{"KILL",9},
	{"SIGKILL",9},
	{"USR1",10},
	{"SIGUSR1",10},
	{"USR2",12},
	{"SIGUSR2",12},
	{"TERM",15},
	{"SIGTERM",15},
	{"CONT",18},
	{"SIGCONT",18},
	{"STOP",19},
	{"SIGSTOP",19},
	{"WINCH",28},
	{"SIGWINCH",28},
	{0,-1}
};

static int sn(const char *s) {
	int i;
	for (i = 0; t[i].n; i++)
		if (se(t[i].n, s))
			return t[i].s;
	return -1;
}

static int ps(const char *s) {
	if (s[0] >= '0' && s[0] <= '9')
		return j(s);
	return sn(s);
}

void _ep(int x, char **z) {
	int s = 15;
	int i = 1;
	if (x < 2) {
		fw(2, "usage: kill <-s SIG|-N> PID...\n");
		sc(1, 1);
	}
	if (z[1][0] == '-') {
		if (z[1][1] == 's' && z[1][2] == 0) {
			if (x < 4) {
				fw(2, "kill: -s requires a signal name or number\n");
				sc(1, 1);
			}
			s = ps(z[2]);
			if (s < 0) {
				fw(2, "kill: unknown signal\n");
				sc(1, 1);
			}
			i = 3;
		} else {
			s = ps(z[1] + 1);
			if (s < 0) {
				fw(2, "kill: unknown signal\n");
				sc(1, 1);
			}
			i = 2;
		}
	}
	if (i >= x) {
		fw(2, "usage: kill <-s SIG|-N> PID...\n");
		sc(1, 1);
	}
	int r = 0;
	while (i < x) {
		int p = j(z[i]);
		int q = o(37, p, s);
		if (q < 0) {
			fw(2, "kill: failed to send signal\n");
			r = 1;
		}
		i++;
	}
	sc(1, r);
}
