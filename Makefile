CC=gcc
BIN=cat echo
ARCH=$(shell uname -m)

all: $(BIN)

$(BIN): %: %.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -static -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clean:
	rm -f $(BIN)
