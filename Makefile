CC=gcc
BIN=cat echo sleep true false
ARCH=$(shell uname -m)

all: $(BIN)

true: true.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		x86_64) \
			$(CROSS) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
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
			$(CROSS) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

$(filter-out false true,$(BIN)): %: %.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -s -o $@ $<; \
			;; \
		x86_64) \
			$(CROSS) -s -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clean:
	rm -f $(BIN)
