CC=cc
CFLAGS=-Wall -Wextra -pedantic -std=gnu99
LFLAGS=-Og -g

CFILES=abbrev.c cgibase.c man2html.c strdefs.c
HFILES=defs.h version.h

all: man2html

man2html: $(CFILES) $(HFILES) Makefile
	$(CC) $(CFLAGS) $(LFLAGS) $(CFILES) -o man2html

clean:
	rm -f man2html

.PHONY: all clean
