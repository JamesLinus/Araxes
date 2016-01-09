// BlacklightEVO kernel/task.c -- x86 software task switching
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <mm.h>
#include <task.h>

struct process_t

int top_pid = 0;

int create_process(void) {
	sbrk(sizeof(page_directory), true);
}
