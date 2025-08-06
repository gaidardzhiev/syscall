#!/bin/sh

fmake() {
	{ [ -f true ]; [ -f false ]; [ -f sleep ]; [ -f echo ]; [ -f cat ]; [ -f sync ]; } && {
			printf "bins exist proceeding with test...\n\n";
			return 0;
		} || {
			printf "bins do not exist running make...\n";
			m=$(make -j8 2>&1 1>/dev/null);
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
    z="hack_the_world"
    x=$(./echo "hack_the_world")
    [ "$x" = "$z" ] && {
		printf "./echo PASSED...\n";
		return 0;
	} || {
		printf "./echo FAILED...\ngot '%s'\nexpected '%s'\n" "$x" "$z";
		return 5;
	}
}

fcat() {
	e="hack_the_world"
	f="/tmp/file"
	printf "%s\n" "$e" > "$f"
	o=$(./cat "$f")
	[ "$o" = "$e" ] && {
		printf "./cat PASSED...\n";
		rm -f "$f";
		return 0;
	} || {
		printf "./cat FAILED...\ngot '%s'\nexpected '%s'\n" "$o" "$e";
		rm -f "$f";
		return 6;
	}
}

fbridge() {
	./bridge 20 > /tmp/q 2>&1 & p=$!
	wait $p
	[ "$(cat /tmp/q)" -eq "$p" ] && {
		printf "./bridge PASSED...\n";
		rm /tmp/q;
		return 0;
	} || {
		printf "./bridge FAILED...\n";
		rm /tmp/q;
		return 7;
	}
}

ftty() {
	z=$(command -v ./tty 2>/dev/null)
	x=$(command -v tty 2>/dev/null)
	[ "$( "$z" )" = "$( "$x" )" ] && {
		printf "./tty PASSED...\n";
		return 0;
	} || {
		printf "./tty FAILED...\n";
		return 8;
	}
}

fsync() {
	strace ./sync 2>&1 | grep -s "sync()" >/dev/null && {
		printf "./sync PASSED...\n";
		return 0;
	} || {
		printf "./sync FAILED...\n";
		return 9;
	}
}

fcrt0() {
	MAIN="/tmp/main.c"
	touch "$MAIN"
	printf "int main(int argc, char **argv){return argc;}" > "$MAIN"
	as -o crt0.o crt0.s
	gcc -c -o main.o "$MAIN"
	gcc -static -nostdlib -e _start -o main crt0.o main.o
	./main arg1 arg2
	RET=$(echo $?)
	[ "$RET" -eq 3 ] && {
		printf "crt0 PASSED...\n";
		rm "$MAIN" crt0.o main.o main
		return 0;
	} || {
		printf "crt0 FAILED...\n";
		rm "$MAIN" crt0.o main.o main
		return 10;
	}
}

#TODO: fshell()

{ fmake && ftrue && ffalse && fsleep && fecho && fcat && fbridge && ftty && fsync && fcrt0; r=$?; } || exit 1

[ "$r" -eq 0 ] 2>/dev/null || printf "%s\n" "$r"
