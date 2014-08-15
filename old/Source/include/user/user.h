// BlacklightEVO user\user.h -- user mode crud
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __USER__USER_H
#define __USER__USER_H

struct process_info {
	int pid;
	int user;

	void* entry;
	void* physical;
	
	struct regs registers;
	unsigned char fxsave[512];

	char name[128];
	char desc[256];
	char cmdline[1024];
};

extern int current_pid;
int get_next_pid(void);

int user_create_process(struct process_info* base);

#endif	// __USER__USER_H