AS=as
CC=gcc
CROSS=arm-linux-gnueabihf-gcc
CFL=-march=armv8-a -marm -std=c99
BIN=cat echo sleep true false bridge tty sync shell test_crt0 id clear pwd uname yes
ARCH=$(shell uname -m)
START=start.S

all: $(BIN)

true: true.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

false: false.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

sync: sync.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

shell: shell.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -fno-stack-protector -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

test_crt0: test_crt0.c
	@case $(ARCH) in \
		armv8l) \
			$(AS) -o crt0.o crt0.s; \
			$(CC) -c -o test_crt0.o test_crt0.c; \
			$(CC) -static -nostdlib -e _start -o test_crt0 crt0.o test_crt0.o; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

id: id.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -static -nostdlib -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clear: clear.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

pwd: pwd.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

uname: uname.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -nostdlib -static -fno-stack-protector -Wl,-e,_ep -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

yes: yes.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		x86_64) \
			$(CC) $(CFL) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

cat: cat.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		x86_64) \
			$(CC) $(CFL) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

echo: echo.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		x86_64) \
			$(CC) $(CFL) -nostdlib -static -fno-stack-protector $(START) -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

$(filter-out false true sync shell test_crt0 id clear touch pwd uname yes cat echo,$(BIN)): %: %.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -s -static -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) $(CFL) -static -s -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clean:
	rm -f $(BIN) crt0.o test_crt0.o
