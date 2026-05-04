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
		[ -f mkdir ];
		[ -f expr ];
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

ftest() {
	./[ 2>/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (no args)\n" "test";
		return 27;
	}
	./[ -z "" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-z empty)\n" "test";
		return 28;
	}
	./[ -z "x" ]
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (-z nonempty)\n" "test";
		return 29;
	}
	./[ -n "x" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-n nonempty)\n" "test";
		return 30;
	}
	./[ -n "" ]
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (-n empty)\n" "test";
		return 31;
	}
	./[ "abc" = "abc" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (string =)\n" "test";
		return 32;
	}
	./[ "abc" != "xyz" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (string !=)\n" "test";
		return 33;
	}
	./[ 42 -eq 42 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-eq)\n" "test";
		return 34
	}
	./[ 1 -ne 2 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-ne)\n" "test";
		return 35;
	}
	./[ 1 -lt 2 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-lt)\n" "test";
		return 36;
	}
	./[ 2 -le 2 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-le)\n" "test";
		return 37;
	}
	./[ 3 -gt 2 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-gt)\n" "test";
		return 38;
	}
	./[ 2 -ge 2 ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-ge)\n" "test";
		return 39;
	}
	./[ -e "/etc/passwd" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-e existing)\n" "test";
		return 40;
	}
	./[ -e "/no/such/file" ]
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (-e missing)\n" "test";
		return 41;
	}
	./[ -f "/etc/passwd" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-f regular)\n" "test";
		return 42;
	}
	./[ -d "/tmp" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-d dir)\n" "test";
		return 43;
	}
	./[ -r "/etc/passwd" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-r readable)\n" "test";
		return 44;
	}
	./[ -x "/bin/sh" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-x executable)\n" "test";
		return 45;
	}
	./[ ! -f "/tmp" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (! negation)\n" "test";
		return 46;
	}
	./[ -f "/etc/passwd" -a -d "/tmp" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-a and)\n" "test";
		return 47;
	}
	./[ -f "/no/such" -o -d "/tmp" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-o or)\n" "test";
		return 48;
	}
	./[ -f "/etc/passwd" 2>/dev/null
	[ "${?}" -eq 2 ] || {
		printf "%-15s FAILED (missing ])\n" "test";
		return 49;
	}
	printf "%-15s PASSED\n" "test"
	return 0
}

fmkdir() {
	rmdir ./syscall_test_dir ./syscall_test_mode ./syscall_a ./syscall_b ./syscall_c 2>/dev/null
	./mkdir 2>/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (no args)\n" "mkdir";
		return 50;
	}
	./mkdir ./syscall_test_dir
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (create dir)\n" "mkdir";
		return 51;
	}
	./[ -d "./syscall_test_dir" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (dir exists)\n" "mkdir";
		return 52;
	}
	./mkdir ./syscall_test_dir 2>/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (already exists)\n" "mkdir";
		return 53;
	}
	./mkdir -m 0700 ./syscall_test_mode
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-m mode)\n" "mkdir";
		return 54;
	}
	./[ -d "./syscall_test_mode" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (-m dir exists)\n" "mkdir";
		return 55;
	}
	./mkdir ./syscall_a ./syscall_b ./syscall_c
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (multiple paths)\n" "mkdir";
		return 56;
	}
	./[ -d "./syscall_a" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (multi path a)\n" "mkdir";
		return 57;
	}
	./[ -d "./syscall_b" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (multi path b)\n" "mkdir";
		return 58;
	}
	./[ -d "./syscall_c" ]
	[ "${?}" -eq 0 ] || {
		printf "%-15s FAILED (multi path c)\n" "mkdir";
		return 59;
	}
	rmdir ./syscall_test_dir ./syscall_test_mode ./syscall_a ./syscall_b ./syscall_c
	printf "%-15s PASSED\n" "mkdir";
	return 0;
}

fexpr() {
	./expr 2>/dev/null
	[ "${?}" -eq 2 ] || {
		printf "%-15s FAILED (no args)\n" "expr";
		return 60;
	}
	[ "$(./expr 1 + 1)" = "2" ] || {
		printf "%-15s FAILED (add)\n" "expr";
		return 61;
	}
	[ "$(./expr 5 - 3)" = "2" ] || {
		printf "%-15s FAILED (sub)\n" "expr";
		return 62;
	}
	[ "$(./expr 3 \* 4)" = "12" ] || {
		printf "%-15s FAILED (mul)\n" "expr";
		return 63;
	}
	[ "$(./expr 10 / 3)" = "3" ] || {
		printf "%-15s FAILED (div)\n" "expr";
		return 64;
	}
	[ "$(./expr 10 % 3)" = "1" ] || {
		printf "%-15s FAILED (mod)\n" "expr";
		return 65;
	}
	./expr 5 / 0 2>/dev/null
	[ "${?}" -eq 2 ] || {
		printf "%-15s FAILED (div zero)\n" "expr";
		return 66;
	}
	[ "$(./expr 2 = 2)" = "1" ] || {
		printf "%-15s FAILED (eq true)\n" "expr";
		return 67;
	}
	./expr 2 = 3 >/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (eq false)\n" "expr";
		return 68;
	}
	[ "$(./expr 2 != 3)" = "1" ] || {
		printf "%-15s FAILED (ne true)\n" "expr";
		return 69;
	}
	[ "$(./expr 1 \< 2)" = "1" ] || {
		printf "%-15s FAILED (lt true)\n" "expr";
		return 70;
	}
	./expr 2 \< 1 >/dev/null
	[ "${?}" -eq 1 ] || {
		printf "%-15s FAILED (lt false)\n" "expr";
		return 71;
	}
	[ "$(./expr 2 \<= 2)" = "1" ] || {
		printf "%-15s FAILED (le true)\n" "expr";
		return 72;
	}
	[ "$(./expr 3 \> 2)" = "1" ] || {
		printf "%-15s FAILED (gt true)\n" "expr";
		return 73;
	}
	[ "$(./expr 2 \>= 2)" = "1" ] || {
		printf "%-15s FAILED (ge true)\n" "expr";
		return 74;
	}
	[ "$(./expr 5 \| 0)" = "5" ] || {
		printf "%-15s FAILED (or lhs)\n" "expr";
		return 75;
	}
	[ "$(./expr 0 \| 7)" = "7" ] || {
		printf "%-15s FAILED (or rhs)\n" "expr";
		return 76;
	}
	[ "$(./expr 3 \& 4)" = "3" ] || {
		printf "%-15s FAILED (and true)\n" "expr";
		return 77;
	}
	[ "$(./expr 0 \& 4)" = "0" ] || {
		printf "%-15s FAILED (and false)\n" "expr";
		return 78;
	}
	[ "$(./expr length hello)" = "5" ] || {
		printf "%-15s FAILED (length)\n" "expr";
		return 79;
	}
	[ "$(./expr length '')" = "0" ] || {
		printf "%-15s FAILED (length empty)\n" "expr";
		return 80;
	}
	[ "$(./expr substr abcdef 2 3)" = "bcd" ] || {
		printf "%-15s FAILED (substr)\n" "expr";
		return 81;
	}
	[ "$(./expr substr abcdef 1 1)" = "a" ] || {
		printf "%-15s FAILED (substr start)\n" "expr";
		return 82;
	}
	[ "$(./expr index abcdef c)" = "3" ] || {
		printf "%-15s FAILED (index hit)\n" "expr";
		return 83;
	}
	[ "$(./expr index abcdef z)" = "0" ] || {
		printf "%-15s FAILED (index miss)\n" "expr";
		return 84;
	}
	[ "$(./expr \( 2 + 3 \) \* 4)" = "20" ] || {
		printf "%-15s FAILED (parens)\n" "expr";
		return 85;
	}
	[ "$(./expr 1 + 2 \* 3)" = "7" ] || {
		printf "%-15s FAILED (precedence)\n" "expr";
		return 86;
	}
	[ "$(./expr -3 + 1)" = "-2" ] || {
		printf "%-15s FAILED (negative)\n" "expr";
		return 87;
	}
	./expr foo + 1 2>/dev/null
	[ "${?}" -eq 2 ] || {
		printf "%-15s FAILED (non-integer)\n" "expr";
		return 88;
	}
	./expr 1 + 2 3 2>/dev/null
	[ "${?}" -eq 2 ] || {
		printf "%-15s FAILED (extra operand)\n" "expr";
		return 89;
	}
	printf "%-15s PASSED\n" "expr"
	return 0
}

{ fmake && ftrue && ffalse && fsleep && fecho && fcat && fbridge && ftty && fsync && fcrt0 && fclear && fpwd && funame && fyes && fwc && fshell && fkill && ftest && fmkdir && fexpr; r="${?}"; } || exit 1

[ "${r}" -eq 0 ] 2>/dev/null || printf "%s\n" "${r}"
