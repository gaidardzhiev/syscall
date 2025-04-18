CC=tcc

BINS=cat

all: $(BINS)

$(BINS): %: %.c
	$(CC) -o $@ $<

clean:
	rm $(BINS)
