CC=tcc

BINS=cat

all: $(BINS)

$(BINS): %: %.c
	$(CC) -o $@ $<

clean:
	rm $(BINS)

.SILENT: man
man:
	echo cat.c - concatenate files and print on the stdout using system calls 
