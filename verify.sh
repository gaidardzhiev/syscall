#!/bin/sh

fmake() {
	{ 
		[ -f true ];
		[ -f false ];
		[ -f sleep ];
		[ -f echo ];
		[ -f cat ];
		[ -f sync ];
		[ -f clear ];
		[ -f pwd ];
		[ -f uname ];
		[ -f yes ];
		[ -f wc ];
		[ -f shell ];
		[ -f kill ];
	} && {
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
	strace ./sync 2>&1 | grep -v "proot warning: ptrace request 'PTRACE_???' not supported yet" | grep -q "sync()" >/dev/null && {
		printf "%-15s PASSED\n" "sync";
		return 0;
	} || {
		printf "%-15s FAILED\n" "sync";
		return 9;
	}
}

fcrt0() {
	main="/tmp/main.c"
	touch "${main}"
	printf "int main(int argc, char **argv){return argc;}" > "${main}"
	as -o crt0.o crt0.s
	gcc -c -o main.o "${main}"
	gcc -static -nostdlib -e _start -o main crt0.o main.o
	./main arg1 arg2
	ret=$(echo ${?})
	[ "${ret}" -eq 3 ] && {
		printf "%-15s PASSED\n" "crt0";
		rm "${main}" crt0.o main.o main
		return 0;
	} || {
		printf "%-15s FAILED\n" "crt0";
		rm "${main}" crt0.o main.o main
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

fpwd() {
	x=$(./pwd 2>/dev/null)
	y=$(pwd 2>/dev/null)
	[ "${x}" = "${y}" ] && {
		printf "%-15s PASSED\n" "pwd";
		return 0;
	} || {
		printf "%-15s FAILED\n" "pwd";
		return 12;
	} 
}

funame() {
	x=$(./uname 2>/dev/null)
	y=$(uname -a 2>/dev/null)
	x=$(echo "${x}" | sed 's/[^ ]* *$//')
	y=$(echo "${y}" | sed 's/[^ ]* *$//')
	[ "${x}" = "${y}" ] && {
		printf "%-15s PASSED\n" "uname";
		return 0;
	} || {
		printf "%-15s FAILED\n" "uname";
		return 13;
	} 
}

fyes() {
	t="4"
	./yes > yes_x.$$ & pid_x="${!}"
	/usr/bin/yes > yes_y.$$ & pid_y="${!}"
	sleep $((t - 1))
	kill "${pid_x}" "${pid_y}" 2>/dev/null
	wait "${pid_x}" "${pid_y}" 2>/dev/null
	x=$(grep -o 'y' yes_x.$$ | head -8 | tr -d '\n')
	y=$(grep -o 'y' yes_y.$$ | head -8 | tr -d '\n')
	rm -f yes_x.$$ yes_y.$$
	[ "$x" = "$y" ] && {
		printf "%-15s PASSED\n" "yes";
		return 0
	} || {
		printf "%-15s FAILED\n" "yes";
		return 14
	}
}

fwc() {
	x=$(./wc wc.c 2>/dev/null | tr -s ' ' | xargs)
	y=$(wc wc.c 2>/dev/null | tr -s ' ' | xargs)
	[ "${x}" = "${y}" ] && {
		printf "%-15s PASSED\n" "wc";
		return 0;
	} || {
		printf "%-15s FAILED\n" "wc";
		return 15;
	} 
}

fshell() {
	printf 'exit\n' | ./shell > /dev/null 2>&1
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (exit code)\n" "shell";
		return 16;
	}
	x=$(printf '/bin/echo hack_the_world\nexit\n' | ./shell 2>/dev/null | tr -d '\r' | sed 's/root# //g' | grep -v '^exit$' | grep -v '^$' | head -1)
	[ "${x}" = "hack_the_world" ] || {
		printf "%-15s FAILED (exec)\ngot '%s'\n" "shell" "${x}";
		return 17;
	}
	x=$(printf 'cd /tmp\n/bin/pwd\nexit\n' | ./shell 2>/dev/null | tr -d '\r' | sed 's/root# //g' | grep -v '^exit$' | grep -v '^$' | head -1)
	[ "${x}" = "/tmp" ] || {
		printf "%-15s FAILED (cd)\ngot '%s'\n" "shell" "${x}";
		return 18;
	}
	printf "%-15s PASSED\n" "shell";
	return 0;
}

fkill() {
	./sleep 60 & p="${!}"
	./kill "${p}"
	wait "${p}" 2>/dev/null
	[ "${?}" -eq 143 ] || {
		printf "%-15s FAILED (SIGTERM default)\n" "kill";
		return 19;
	}
	./sleep 60 & p="${!}"
	./kill -9 "${p}"
	wait "${p}" 2>/dev/null
	[ "${?}" -eq 137 ] || {
		printf "%-15s FAILED (-9 shorthand)\n" "kill";
		return 20;
	}
	./sleep 60 & p="${!}"
	./kill -s 9 "${p}"
	wait "${p}" 2>/dev/null
	[ "${?}" -eq 137 ] || {
		printf "%-15s FAILED (-s numeric)\n" "kill";
		return 21;
	}
	./sleep 60 & p="${!}"
	./kill -s KILL "${p}"
	wait "${p}" 2>/dev/null
	[ "${?}" -eq 137 ] || {
		printf "%-15s FAILED (-s name)\n" "kill";
		return 22;
	}
	./sleep 60 & p="${!}"
	./kill -TERM "${p}"
	wait "${p}" 2>/dev/null
	[ "${?}" -eq 143 ] || {
		printf "%-15s FAILED (-SIGNAME shorthand)\n" "kill";
		return 23;
	}
	./sleep 60 & p1="${!}"
	./sleep 60 & p2="${!}"
	./kill -9 "${p1}" "${p2}"
	wait "${p1}" "${p2}" 2>/dev/null
	[ "${?}" -eq 137 ] || {
		printf "%-15s FAILED (multiple PIDs)\n" "kill";
		return 24;
	}
	./kill -s DEADBEEF 1 2>/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (bad signal)\n" "kill";
		return 25;
	}
	./kill 2>/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (no args)\n" "kill";
		return 26;
	}
	printf "%-15s PASSED\n" "kill"
	return 0
}

{ fmake && ftrue && ffalse && fsleep && fecho && fcat && fbridge && ftty && fsync && fcrt0 && fclear && fpwd && funame && fyes && fwc && fshell && fkill; r="${?}"; } || exit 1

[ "${r}" -eq 0 ] 2>/dev/null || printf "%s\n" "${r}"
