CC=musl-gcc
CFL=-fno-stack-protector

yes: yes.c start.S
	$(CC) -nostdlib -static $(CFL) start.S -o $@ yes.c

clean:
	rm -f yes
