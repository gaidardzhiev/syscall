CC=musl-gcc
CFL=-fno-stack-protector

all: yes test

yes: yes.c start.S
	$(CC) -nostdlib -static $(CFL) start.S -o $@ yes.c

test: test.c start.S
	$(CC) -nostdlib -static $(CFL) start.S -o $@ test.c


clean:
	rm -f yes test
