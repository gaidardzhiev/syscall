AS=as
CC=gcc
CROSS=arm-linux-gnueabihf-gcc
CFL=-march=armv8-a -marm -std=c99
BIN=cat echo sleep true false bridge tty sync shell test_crt0 id
ARCH=$(shell uname -m)

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
			$(CC) -nostdlib -static -fno-stack-protector -o $@ $<; \
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
			$(CC) -static -nostdlib -fno-stack-protector -Wl,-e,_start -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac


$(filter-out false true sync shell test_crt0 id,$(BIN)): %: %.c
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
