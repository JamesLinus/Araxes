// BlacklightEVO hardware\layouts\dvorak.cpp -- Dvorak keyboard layout
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "hardware\keyboard.h"

unsigned char set1_dvorak_map[] = {
/* 00 */0,	0x1B,	'1',	'2',	'3',	'4',	'5',	'6',
/* 08 */'7',	'8',	'9',	'0',	'[',	']',	'\b',	'\t',
/* 10 */'\'',	',',	'.',	'p',	'y',	'f',	'g',	'c',
/* 1Dh is left Ctrl */
/* 18 */'r',	'l',	'/',	'=',	'\n',	0,	'a',	'o',
/* 20 */'e',	'u',	'i',	'd',	'h',	't',	'n',	's',
/* 2Ah is left Shift */
/* 28 */'-',	'`',	0,	'\\',	';',	'q',	'j',	'k',
/* 36h is right Shift */
/* 30 */'x',	'b',	'm',	'w',	'v',	'z',	0,	0,
/* 38h is left Alt, 3Ah is Caps Lock */
/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
/* 45h is Num Lock, 46h is Scroll Lock */
/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
/* 58 */KEY_F12
};

unsigned char set1_dvorak_shift_map[] = {
/* 00 */0,	0x1B,	'!',	'@',	'#',	'$',	'%',	'^',
/* 08 */'&',	'*',	'(',	')',	'{',	'}',	'\b',	'\t',
/* 10 */'"',	'<',	'>',	'P',	'Y',	'F',	'G',	'C',
/* 1Dh is left Ctrl */
/* 18 */'R',	'L',	'?',	'+',	'\n',	0,	'A',	'O',
/* 20 */'E',	'U',	'I',	'D',	'H',	'T',	'N',	'S',
/* 2Ah is left Shift */
/* 28 */'_',	'~',	0,	'|',	':',	'Q',	'J',	'K',
/* 36h is right Shift */
/* 30 */'X',	'B',	'M',	'W',	'V',	'Z',	0,	0,
/* 38h is left Alt, 3Ah is Caps Lock */
/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
/* 45h is Num Lock, 46h is Scroll Lock */
/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
/* 58 */KEY_F12
};