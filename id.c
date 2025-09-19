/*
 * Copyright (C) 2025 Ivan Gaydardzhiev
 * Licensed under the GPL-3.0-only
 */

struct u{
	char x[65];
	char z[65];
	char f[65];
	char g[65];
	char t[65];
};

static inline int b(int w) {
	register int q asm("r7")=w;
	register int n asm("r0");
	asm volatile("svc 0" : "=r"(n) : "r"(q) : "memory");
	return n;
}

static inline int p(int w,int s) {
	register int q asm("r7")=w;
	register int n asm("r0")=s;
	asm volatile("svc 0" : "+r"(n) : "r"(q) : "memory");
	return n;
}

static inline int y(int w,int s,int r,int v) {
	register int q asm("r7")=w;
	register int n asm("r0")=s;
	register int m asm("r1")=r;
	register int o asm("r2")=v;
	asm volatile("svc 0" : "+r"(n) : "r"(m),"r"(o),"r"(q) : "memory");
	return n;
}

static void a(const char *c,int d) {
	y(4,1,(int)c,d);
}

static void j(char k) {
	y(4,1,(int)&k,1);
}

static void e(unsigned int h) {
	char i[11];
	int l=10;
	i[l]=0;
	if(h==0)
		i[--l]='0';
	while(h>0&&l>0)
	{
		i[--l]='0'+(h%10);
		h/=10;
	}
	a(i+l,10-l);
}

static void m(const char *c,unsigned int h) {
	int d=0;
	while(c[d]) d++;
	a(c,d);
	e(h);
	j('\n');
}

static void r(const char *c,const char *d) {
	int d1=0;
	while(c[d1]) d1++;
	a(c,d1);
	d1=0;
	while(d[d1]) d1++;
	a(d,d1);
	j('\n');
}

int _start() {
	int s;
	char **w;
	unsigned int q=0,n=0,s_var=0,g=0;
	int f=0;
	unsigned int t[16];
	struct u p1;
	int z;
	asm volatile("mov %0, sp" : "=r"(w));
	s = *(int*)w;
	w = (char**)(w + 1);
	q=b(24);
	n=b(47);
	s_var=b(49);
	g=b(50);
	f=y(80,16,(int)t,0);
	y(122,(int)&p1,0,0);
	if(s>0)
		r("prog=",w[0]);
	m("uid=",q);
	m("gid=",n);
	m("euid=",s_var);
	m("egid=",g);
	m("groups=",f);
	r("sysname=",p1.x);
	r("nodename=",p1.z);
	r("release=",p1.f);
	r("version=",p1.g);
	r("machine=",p1.t);
	a("group list:\n",11);
	for(z=0;z<f&&z<16;z++) {
		e(t[z]);
		j('\n');
	}
	a("\n",1);
	p(1,0);
	while(1);
}
