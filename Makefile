CC=gcc
BIN=cat echo sleep false
ARCH=$(shell uname -m)

all: $(BIN)

false: false.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -nostdlib -static -o $@ $< -Wl,-e,_ep; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

$(filter-out false,$(BIN)): %: %.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -s -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clean:
	rm -f $(BIN)
