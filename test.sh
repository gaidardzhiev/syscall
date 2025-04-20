#!/bin/sh

fmake() {
	{ [ -f false ]; [ -f sleep ]; [ -f echo ]; } && {
			printf "bins exist proceeding with test...\n";
			return 0;
		} || {
			printf "bins do not exist running make...\n";
			make;
		}
}

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

fecho() {
    z="hack the world"
    x=$(./echo "hack the world")
    [ "$x" = "$z" ] && {
		printf "./echo PASSED...\n";
		return 0;
	} || {
		printf "./echo FAILED...\ngot '%s'\nexpected '%s'\n" "$x" "$z";
		return 4;
	}
}

fcat() {
	e="hack the world"
	f="/tmp/file"
	printf "%s\n" "$e" > "$f"
	o=$(./cat "$f")
	[ "$o" = "$e" ] && {
		printf "./cat PASSED...\n";
		rm -f "$f"
		return 0;
	} || {
		printf "./cat FAILED...\ngot '%s'\nexpected '%s'\n" "$o" "$e";
		rm -f "$f"
		return 5;
	}
}


{ fmake && ffalse && fsleep && fecho && fcat; RET=$?; } || exit 1

#[ "$RET" -ne 0 ] && printf "%d\n" "$RET"
