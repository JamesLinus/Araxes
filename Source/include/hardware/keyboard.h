// BlacklightEVO hardware\keyboard.h -- PS/2 keyboard
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Code originally pillaged from Chris Giese's example PS/2 keyboard driver

#ifndef __HARDWARE__KEYBOARD_H
#define __HARDWARE__KEYBOARD_H

/*----------------------------------------------------------------------------
SCANCODE CONVERSION
----------------------------------------------------------------------------*/
/* "ASCII" values for non-ASCII keys. All of these are user-defined.
function keys: */
#define	KEY_F1		0x80
#define	KEY_F2		(KEY_F1 + 1)
#define	KEY_F3		(KEY_F2 + 1)
#define	KEY_F4		(KEY_F3 + 1)
#define	KEY_F5		(KEY_F4 + 1)
#define	KEY_F6		(KEY_F5 + 1)
#define	KEY_F7		(KEY_F6 + 1)
#define	KEY_F8		(KEY_F7 + 1)
#define	KEY_F9		(KEY_F8 + 1)
#define	KEY_F10		(KEY_F9 + 1)
#define	KEY_F11		(KEY_F10 + 1)
#define	KEY_F12		(KEY_F11 + 1)
/* cursor keys */
#define	KEY_INS		0x90
#define	KEY_DEL		(KEY_INS + 1)
#define	KEY_HOME	(KEY_DEL + 1)
#define	KEY_END		(KEY_HOME + 1)
#define	KEY_PGUP	(KEY_END + 1)
#define	KEY_PGDN	(KEY_PGUP + 1)
#define	KEY_LFT		(KEY_PGDN + 1)
#define	KEY_UP		(KEY_LFT + 1)
#define	KEY_DN		(KEY_UP + 1)
#define	KEY_RT		(KEY_DN + 1)
/* print screen/sys rq and pause/break */
#define	KEY_PRNT	(KEY_RT + 1)
#define	KEY_PAUSE	(KEY_PRNT + 1)
/* these return a value but they could also act as additional meta keys */
#define	KEY_LWIN	(KEY_PAUSE + 1)
#define	KEY_RWIN	(KEY_LWIN + 1)
#define	KEY_MENU	(KEY_RWIN + 1)

/* "meta bits"
0x0100 is reserved for non-ASCII keys, so start with 0x200 */
#define	KBD_META_ALT	0x0200	/* Alt is pressed */
#define	KBD_META_CTRL	0x0400	/* Ctrl is pressed */
#define	KBD_META_SHIFT	0x0800	/* Shift is pressed */
#define	KBD_META_ANY	(KBD_META_ALT | KBD_META_CTRL | KBD_META_SHIFT)
#define	KBD_META_CAPS	0x1000	/* CapsLock is on */
#define	KBD_META_NUM	0x2000	/* NumLock is on */
#define	KBD_META_SCRL	0x4000	/* ScrollLock is on */
/*****************************************************************************
*****************************************************************************/
#define	RAW1_LEFT_CTRL		0x1D
#define	RAW1_LEFT_SHIFT		0x2A
#define	RAW1_CAPS_LOCK		0x3A
#define	RAW1_LEFT_ALT		0x38
#define	RAW1_RIGHT_ALT		0x38	/* same as left */
#define	RAW1_RIGHT_CTRL		0x1D	/* same as left */
#define	RAW1_RIGHT_SHIFT	0x36
#define	RAW1_SCROLL_LOCK	0x46
#define	RAW1_NUM_LOCK		0x45
#define	RAW1_DEL		0x53

extern unsigned char set1_qwerty_map[];
extern unsigned char set1_qwerty_shift_map[];
extern unsigned char set1_dvorak_map[];
extern unsigned char set1_dvorak_shift_map[];

extern unsigned char* keyboard_map;
extern unsigned char* keyboard_shift_map;

#define keyboard_initialize() init_kbd(2, 0, 1)

int init_kbd(unsigned ss, unsigned typematic, unsigned xlat);
void keyboard_interrupt(void);
unsigned char keyboard_getchar();

#endif	// __HARDWARE__KEYBOARD_H