#!/bin/sh

ffalse() {
	./false && {
		printf "./false FAILED...\n";
		return 2;
	} || {
		printf "./false PASSED...\n";
		return 0;
	}
}

fsleep() {
	s=$(date +%s)
	./sleep 1 && {
		e=$(date +%s);
		t=$((e - s));
		[ "$t" -eq 1 ];
	} && {
		printf "./sleep PASSED...\n";
		return 0;
	} || {
		printf "./sleep FAILED...\n";
		return 3;
	}
}



{ ffalse && fsleep; RET=$?; } || exit 1

[ "$RET" -ne 0 ] && printf "%d\n" "$RET"
