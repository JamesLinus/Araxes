// BlacklightEVO core\exceptions.cpp -- x86 CPU exception handlers
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#ifndef __CORE__EXCEPTIONS_H
#define __CORE__EXCEPTIONS_H

void exception_int00(void);						// Divide Error
void exception_int02(void);						// Non-Maskable Interrupt
void exception_int04(void);						// Overflow
void exception_int05(void);						// Bound Range Exceeded
void exception_int06(void);						// Invalid Opcode
void exception_int07(void);						// Device Not Available
void exception_int08(void);						// Double Fault
void exception_int09(void);						// Coprocessor Segment Overrun
void exception_int0A(void);						// Invalid TSS
void exception_int0B(void);						// Segment Not Present
void exception_int0C(void);						// Stack Segment Fault
void exception_int0D(void);						// General Protection Fault
void exception_int10(void);						// Floating-Point Exception

#endif  // __CORE__EXCEPTIONS_H