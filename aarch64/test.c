/*
 * Copyright (C) 2026 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

static void fw(const char *s) {
        int i = 0;
        while (s[i]) i++;
        register long x8 asm("x8") = 64;/* write */
        register long x0 asm("x0") = 2;
        register long x1 asm("x1") = (long)s;
        register long x2 asm("x2") = i;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
                        : "memory");
}

static void x(int c) {
        register long x8 asm("x8") = 93;/* exit */
        register long x0 asm("x0") = c;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0)
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

static long long u64(const char *p) {
        const unsigned char *b = (const unsigned char *)p;
        return (long long)(
               ((unsigned long long)b[0]) |
               ((unsigned long long)b[1] << 8) |
               ((unsigned long long)b[2] << 16) |
               ((unsigned long long)b[3] << 24) |
               ((unsigned long long)b[4] << 32) |
               ((unsigned long long)b[5] << 40) |
               ((unsigned long long)b[6] << 48) |
               ((unsigned long long)b[7] << 56));
}

static int ismode(unsigned int m) {
        unsigned int t = m & 0170000;
        return t == 0100000 || t == 0040000 || t == 0120000 ||
               t == 0060000 || t == 0020000 || t == 0010000 ||
               t == 0140000;
}

static int sys_access(const char *p, int m) {
        register long x8 asm("x8") = 21;/* faccessat */
        register long x0 asm("x0") = -100;/* AT_FDCWD */
        register long x1 asm("x1") = (long)p;
        register long x2 asm("x2") = m;
        register long x3 asm("x3") = 0;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3)
                        : "memory");
        return (int)x0;
}

static int sys_open(const char *p, int f, int m) {
        register long x8 asm("x8") = 56;/* openat */
        register long x0 asm("x0") = -100;/* AT_FDCWD */
        register long x1 asm("x1") = (long)p;
        register long x2 asm("x2") = f;
        register long x3 asm("x3") = m;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3)
                        : "memory");
        return (int)x0;
}

static int sys_close(int fd) {
        register long x8 asm("x8") = 57;/* close */
        register long x0 asm("x0") = fd;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0)
                        : "memory");
        return (int)x0;
}

static long sys_lseek(int fd, long off, int wh) {
        register long x8 asm("x8") = 62;/* lseek */
        register long x0 asm("x0") = fd;
        register long x1 asm("x1") = off;
        register long x2 asm("x2") = wh;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0), "r"(x1), "r"(x2)
                        : "memory");
        return x0;
}

static int fsize_pos(const char *p) {
        int fd = sys_open(p, 0, 0);
        long s;
        if (fd < 0) return 0;
        s = sys_lseek(fd, 0, 2);
        sys_close(fd);
        return s > 0;
}

/*
 * AArch64 stat struct (struct statx / fstatat):
 * use newfstatat (79), the kernel fills struct stat64 compatible layout, AArch64 stat offsets:
 * +0: st_dev (8 bytes)
 * +8: st_ino (8 bytes)
 * +16: st_mode (4 bytes)
 * +20: st_nlink (4 bytes)
 * +24: st_uid (4 bytes)
 * +28: st_gid (4 bytes)
 * +32: st_rdev (8 bytes)
 * +40: __pad1 (8 bytes)
 * +48: st_size (8 bytes)
 */
static int fs(const char *p, unsigned int *mode, long long *size) {
        char b[144];
        register long x8 asm("x8") = 79;/* newfstatat */
        register long x0 asm("x0") = -100;/* AT_FDCWD */
        register long x1 asm("x1") = (long)p;
        register long x2 asm("x2") = (long)b;
        register long x3 asm("x3") = 0;
        asm volatile ("svc #0"
                        : "=r"(x0)
                        : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3)
                        : "memory");
        if (x0 != 0) return (int)x0;
        if (mode) *mode = u32(b + 16);
        if (size) *size = u64(b + 48);
        return 0;
}

static int e(int c, char **v);
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
