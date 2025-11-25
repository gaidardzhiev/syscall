#!/bin/sh

fmake() {
	{ [ -f true ]; [ -f false ]; [ -f sleep ]; [ -f echo ]; [ -f cat ]; [ -f sync ]; [ -f clear ]; } && {
			printf "bins exist proceeding with test...\n\n";
			return 0;
		} || {
			printf "bins do not exist running make...\n";
			m=$(make -j8 2>&1 1>/dev/null);
			printf "%s\n" "${m}" >&2;
		}
}

ftrue() {
	./true && {
		printf "%-15s PASSED\n" "true";
		return 0;
	} || {
		printf "%-15s FAILED\n" "true";
		return 2;
	}
}

ffalse() {
	./false && {
		printf "%-15s FAILED\n" "false";
		return 3;
	} || {
		printf "%-15s PASSED\n" "false";
		return 0;
	}
}

fsleep() {
	s=$(date +%s)
	./sleep 1 && {
		e=$(date +%s);
		t=$((e - s));
		[ "${t}" -eq 1 ];
	} && {
		printf "%-15s PASSED\n" "sleep";
		return 0;
	} || {
		printf "%-15s FAILED\n" "sleep";
		return 4;
	}
}

fecho() {
    z="hack_the_world"
    x=$(./echo "hack_the_world")
    [ "${x}" = "${z}" ] && {
		printf "%-15s PASSED\n" "echo";
		return 0;
	} || {
		printf "%-15s FAILED\ngot '%s'\nexpected '%s'\n" "echo" "${x}" "${z}";
		return 5;
	}
}

fcat() {
	e="hack_the_world"
	f="/tmp/file"
	printf "%s\n" "${e}" > "${f}"
	o=$(./cat "${f}")
	[ "${o}" = "${e}" ] && {
		printf "%-15s PASSED\n" "cat";
		rm -f "${f}";
		return 0;
	} || {
		printf "%-15s FAILED...\ngot '%s'\nexpected '%s'\n" "cat" "${o}" "${e}";
		rm -f "${f}";
		return 6;
	}
}

fbridge() {
	./bridge 20 > /tmp/q 2>&1 & p="${!}"
	wait "${p}"
	[ "$(cat /tmp/q)" -eq "${p}" ] && {
		printf "%-15s PASSED\n" "bridge";
		rm /tmp/q;
		return 0;
	} || {
		printf "%-15s FAILED\n" "bridge";
		rm /tmp/q;
		return 7;
	}
}

ftty() {
	z=$(command -v ./tty 2>/dev/null)
	x=$(command -v tty 2>/dev/null)
	[ "$( "${z}" )" = "$( "${x}" )" ] && {
		printf "%-15s PASSED\n" "tty";
		return 0;
	} || {
		printf "%-15s FAILED\n" "tty";
		return 8;
	}
}

fsync() {
#	strace ./sync 2>&1 | grep -s "sync()" >/dev/null && {
	strace ./sync 2>&1 | grep -v "proot warning: ptrace request 'PTRACE_???' not supported yet" | grep -q "sync()" >/dev/null && {
		printf "%-15s PASSED\n" "sync";
		return 0;
	} || {
		printf "%-15s FAILED\n" "sync";
		return 9;
	}
}

fcrt0() {
	MAIN="/tmp/main.c"
	touch "${MAIN}"
	printf "int main(int argc, char **argv){return argc;}" > "${MAIN}"
	as -o crt0.o crt0.s
	gcc -c -o main.o "${MAIN}"
	gcc -static -nostdlib -e _start -o main crt0.o main.o
	./main arg1 arg2
	RET=$(echo ${?})
	[ "${RET}" -eq 3 ] && {
		printf "%-15s PASSED\n" "crt0";
		rm "${MAIN}" crt0.o main.o main
		return 0;
	} || {
		printf "%-15s FAILED\n" "crt0";
		rm "${MAIN}" crt0.o main.o main
		return 10;
	}
}

fclear() {
	strace ./clear 2>&1 | grep -q "write(1" >/dev/null && {
		printf "%-15s PASSED\n" "clear";
		return 0;
	} || {
		printf "%-15s FAILED\n" "clear";
		return 11;
	}
}

#TODO: fshell()

{ fmake && ftrue && ffalse && fsleep && fecho && fcat && fbridge && ftty && fsync && fcrt0 && fclear; r="${?}"; } || exit 1

[ "${r}" -eq 0 ] 2>/dev/null || printf "%s\n" "${r}"
