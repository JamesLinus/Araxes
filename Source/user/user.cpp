// BlacklightEVO user\user.cpp -- user mode crud
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\gdt.h"
#include "user\user.h"

int current_pid = 1;							// The kernel is PID 1

int get_next_pid() {
	return current_pid++;
}

int user_create_process(struct process_info* base);