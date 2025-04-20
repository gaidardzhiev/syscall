#!/bin/sh

fmake() {
	{ [ -f true ]; [ -f false ]; [ -f sleep ]; [ -f echo ]; [ -f cat ]; } && {
			printf "bins exist proceeding with test...\n\n";
			return 0;
		} || {
			printf "bins do not exist running make...\n";
			m=$(make 2>&1 1>/dev/null);
			printf "%s\n" "$m" >&2;
		}
}

ftrue() {
	./true && {
		printf "./true PASSED...\n";
		return 0;
	} || {
		printf "./true FAILED...\n";
		return 2;
	}
}

ffalse() {
	./false && {
		printf "./false FAILED...\n";
		return 3;
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
		return 4;
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
		return 5;
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
		return 6;
	}
}


{ fmake && ftrue && ffalse && fsleep && fecho && fcat; r=$?; } || exit 1

[ "$r" -eq 0 ] 2>/dev/null || printf "%s\n" "$r"
