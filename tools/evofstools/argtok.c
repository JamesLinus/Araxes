// BlacklightEVO tools/evofstools/argtok.c -- Argument tokenizer (think strtok for command lines)
// Copyright (c) 2013-2014 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	char command_input[150];
	char* commandv[32];
	
	for (;;) {
		fgets(command_input, 150, stdin);
	}
}
