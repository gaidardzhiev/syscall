#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/syscall.h>

#define _(O,o) long O##o; for(O##o=0; O##o<6 && O##o+2<z; O##o++) v[O##o] = strtol(x[O##o+2], 0, 0)
#define __ int
#define ___ long
#define ____ (void)
#define _____ return

int main(__ z, char **x) {
	if (z < 2) {
		fprintf(stderr,"usage: %s <syscall_number> <arg1 <arg2 ...>>\n",x[0]);
		_____ 1;
	}
	___ v[6] = {0};
	_(e,);
	___ f = syscall(strtol(x[1], 0, 0), v[0], v[1], v[2], v[3], v[4], v[5]);
	if (f == -1) {
		perror("syscall");
		_____ errno;
	}
	printf("%ld\n", f);
	_____ 0;
}
