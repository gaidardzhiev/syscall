CC=tcc
BINS=cat
ARCH=$(shell uname -m)

all: $(BINS)

$(BINS): %: %.c
	@case $(ARCH) in \
		armv8l) \
			$(CC) -o $@ $<; \
			;; \
		*) \
			printf "unsupported architecture $(ARCH)\n"; \
			exit 1; \
			;; \
	esac

clean:
	rm -f $(BINS)
