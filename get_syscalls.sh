#!/bin/sh

case "$(uname)" in
	Linux)
		p=__NR_ ;;
	*)
		p=SYS_ ;;
esac

command -v cpp > /dev/null || {
	printf "cpp preprocessor not found...\n" >&2;
	exit 1;
}

cpp -include sys/syscall.h -dM </dev/null \
       	| sed -n "s/^#define $p//p" \
       	| sort -k1 || {
		printf "something's wrong in here somewhere...\n" >&2;
		exit 1;
}
