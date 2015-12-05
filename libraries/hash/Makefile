## BlacklightEVO libraries/hash/Makefile
## Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
## The Blacklight project is under the terms of the ISC license. See license.md for details.
## Questions? Comments? Concerns? Email us: blacklight@cordilon.net

SOURCES_C := sha1.c pbkdf2.c base64.c
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
