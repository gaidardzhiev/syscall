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

{ ffalse; RET=$?; } || exit 1

[ "$RET" -ne 0 ] && printf "%d\n" "$RET"
