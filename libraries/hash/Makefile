## BlacklightEVO libraries/hash/Makefile
## Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
## Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
## Questions? Comments? Concerns? Email us: blacklight@cordilon.net

SOURCES_C := sha1.c
OBJECTS := $(SOURCES_C:.c=.o)
OUTFILE = hash.a

CFLAGS = -c -ffreestanding -std=gnu11 -O0 -Wall -Wextra

.PHONY: all clean announce

announce:
	@echo "     - $(DIRECTORY)$(OUTFILE)"

all: announce $(OUTFILE)

clean:
	@rm -f $(OBJECTS) $(OUTFILE) 2>/dev/null

$(OUTFILE): $(OBJECTS)
	@ar rcs $(OUTFILE) $(OBJECTS)

.c.o:
	@echo "       - $<"
	@$(CC) $(CFLAGS) -o $@ $<
