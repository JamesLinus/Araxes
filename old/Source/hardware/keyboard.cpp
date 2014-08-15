// BlacklightEVO hardware\keyboard.cpp -- PS/2 keyboard
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Code originally pillaged from Chris Giese's example PS/2 keyboard driver

#include "global.h"
#include "core\console.h"
#include "core\cpu.h"
#include "hardware\keyboard.h"

unsigned char set1_qwerty_map[] = {
/* 00 */0,	0x1B,	'1',	'2',	'3',	'4',	'5',	'6',
/* 08 */'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10 */'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 1Dh is left Ctrl */
/* 18 */'o',	'p',	'[',	']',	'\n',	0,	'a',	's',
/* 20 */'d',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 2Ah is left Shift */
/* 28 */'\'',	'`',	0,	'\\',	'z',	'x',	'c',	'v',
/* 36h is right Shift */
/* 30 */'b',	'n',	'm',	',',	'.',	'/',	0,	'*',
/* 38h is left Alt, 3Ah is Caps Lock */
/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
/* 45h is Num Lock, 46h is Scroll Lock */
/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
/* 58 */KEY_F12
};

unsigned char set1_qwerty_shift_map[] = {
/* 00 */0,	0x1B,	'!',	'@',	'#',	'$',	'%',	'^',
/* 08 */'&',	'*',	'(',	')',	'_',	'+',	'\b',	'\t',
/* 10 */'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 1Dh is left Ctrl */
/* 18 */'O',	'P',	'{',	'}',	'\n',	0,	'A',	'S',
/* 20 */'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
/* 2Ah is left Shift */
/* 28 */'"',	'~',	0,	'|',	'Z',	'X',	'C',	'V',
/* 36h is right Shift */
/* 30 */'B',	'N',	'M',	'<',	'>',	'?',	0,	'*',
/* 38h is left Alt, 3Ah is Caps Lock */
/* 38 */0,	' ',	0,	KEY_F1,	KEY_F2,	KEY_F3,	KEY_F4,	KEY_F5,
/* 45h is Num Lock, 46h is Scroll Lock */
/* 40 */KEY_F6,	KEY_F7,	KEY_F8,	KEY_F9,	KEY_F10,0,	0,	KEY_HOME,
/* 48 */KEY_UP,	KEY_PGUP,'-',	KEY_LFT,'5',	KEY_RT,	'+',	KEY_END,
/* 50 */KEY_DN,	KEY_PGDN,KEY_INS,KEY_DEL,0,	0,	0,	KEY_F11,
/* 58 */KEY_F12
};

unsigned char* keyboard_map = set1_qwerty_map;
unsigned char* keyboard_shift_map = set1_qwerty_shift_map;

int keyboard_map_size = sizeof(set1_qwerty_map);
int keyboard_shift_map_size = sizeof(set1_qwerty_shift_map);

bool keyboard_initialized = false;

int keyboard_timeout = 20000;

typedef struct
{
	unsigned char *data;
	unsigned size, in_ptr, out_ptr;
} queue_t;
/*****************************************************************************
*****************************************************************************/
static int inq(queue_t *q, unsigned data)
{
	unsigned temp;

	temp = q->in_ptr + 1;
	if(temp >= q->size)
		temp = 0;
/* if in_ptr reaches out_ptr, the queue is full */
	if(temp == q->out_ptr)
		return -1;
	q->data[q->in_ptr] = data;
	q->in_ptr = temp;
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int deq(queue_t *q, unsigned char *data)
{
/* if out_ptr reaches in_ptr, the queue is empty */
	if(q->out_ptr == q->in_ptr)
		return -1;
	*data = q->data[q->out_ptr++];
	if(q->out_ptr >= q->size)
		q->out_ptr = 0;
	return 0;
}
/*****************************************************************************
*****************************************************************************/
static int empty(queue_t *q)
{
	return q->out_ptr == q->in_ptr;
}
/*----------------------------------------------------------------------------
LOW-LEVEL KEYBOARD DRIVER
----------------------------------------------------------------------------*/
#define	BUF_SIZE	64

static unsigned char g_kbd_buf[BUF_SIZE];
static queue_t g_queue =
{
	g_kbd_buf, BUF_SIZE, 0, 0
};
/*****************************************************************************
*****************************************************************************/
void keyboard_interrupt(void)
{
	if (!keyboard_initialized)
		return;
	//printf("Handled kbd IRQ\n");
	unsigned scan_code;

/* read I/O port 60h to reset interrupt at 8042 keyboard controller chip */
	scan_code = inb(0x60);
/* put scancode in queue */
	(void)inq(&g_queue, scan_code);
/* reset interrupt at 8259 interrupt controller chip */
}
/*****************************************************************************
*****************************************************************************/
static int read_kbd(void)
{
	unsigned long timeout;
	unsigned stat, data;

	for(timeout = keyboard_timeout; timeout != 0; timeout--)
	{
		stat = inb(0x64);
/* loop until 8042 output buffer full */
		if(stat & 0x01)
		{
			data = inb(0x60);
/* loop if parity error or receive timeout */
			if((stat & 0xC0) == 0)
				return data;
		}
	}
	//	printf("read_kbd: timeout\n");
	return -1;
}
/*****************************************************************************
*****************************************************************************/
static void write_kbd(unsigned adr, unsigned data)
{
	unsigned long timeout;
	unsigned stat;

	for(timeout = keyboard_timeout; timeout != 0; timeout--)
	{
		stat = inb(0x64);
/* loop until 8042 input buffer empty */
		if((stat & 0x02) == 0)
			break;
	}
	if(timeout == 0)
	{
		printf("write_kbd: timeout\n");
		return;
	}
	outb(adr, data);
}
/*****************************************************************************
*****************************************************************************/
static int write_kbd_await_ack(unsigned val)
{
	int got;

	write_kbd(0x60, val);
	got = read_kbd();
	if(got != 0xFA)
	{
		printf("write_kbd_await_ack: expected "
			"acknowledge (0xFA), got 0x%02X\n", got);
		return -1;
	}
	return 0;
}
/*****************************************************************************
*****************************************************************************/
int init_kbd(unsigned ss, unsigned typematic, unsigned xlat)
{
	//printf("flushing keyboard output\n");
	while(read_kbd() != -1)
		/* nothing */;
/* disable keyboard before programming it */
	//printf("disabling keyboard controller\n");
	if(write_kbd_await_ack(0xF5) != 0)
		return -1;
/* disable PS/2 mouse, set SYS bit, and Enable Keyboard Interrupt... */
	write_kbd(0x64, 0x60);
/* ...and either disable or enable AT-to-XT keystroke conversion */
	write_kbd(0x60, xlat ? 0x65 : 0x25);
/* program desired scancode set */
	//printf("programming scancode set %u\n", ss);
	if(write_kbd_await_ack(0xF0) != 0)
		return -1;
	if(write_kbd_await_ack(ss) != 0)
		return -1;
/* we want all keys to return both a make code (when pressed)
and a break code (when released -- scancode set 3 only) */
	if(ss == 3)
	{
		//printf("making all keys make-break\n");
		if(write_kbd_await_ack(0xFA) != 0)
			return -1;
	}
/* set typematic delay and rate */
	//printf("setting fast typematic mode\n");
	if(write_kbd_await_ack(0xF3) != 0)
		return -1;
	if(write_kbd_await_ack(typematic) != 0)
		return -1;
/* enable keyboard */
	//printf("enabling keyboard controller\n");
	if(write_kbd_await_ack(0xF4) != 0)
		return -1;

	keyboard_initialized = true;
	return 0;
}

static int set1_handle_E0(unsigned code) {
	switch (code) {
		//case 
	}
	return 0;
}

static int set1_scancode_to_ascii(unsigned code)
{
	static unsigned saw_break_code, kbd_status;
/**/
	unsigned temp;

/* check for break code (i.e. a key is released) */
	if(code >= 0x80)
	{
		saw_break_code = 1;
		code &= 0x7F;
	}
/* the only break codes we're interested in are Shift, Ctrl, Alt */
	if(saw_break_code)
	{
		if(code == RAW1_LEFT_ALT || code == RAW1_RIGHT_ALT)
			kbd_status &= ~KBD_META_ALT;
		else if(code == RAW1_LEFT_CTRL || code == RAW1_RIGHT_CTRL)
			kbd_status &= ~KBD_META_CTRL;
		else if(code == RAW1_LEFT_SHIFT || code == RAW1_RIGHT_SHIFT)
			kbd_status &= ~KBD_META_SHIFT;
		saw_break_code = 0;
		return -1;
	}
/* it's a make code: check the "meta" keys, as above */
	if(code == RAW1_LEFT_ALT || code == RAW1_RIGHT_ALT)
	{
		kbd_status |= KBD_META_ALT;
		return -1;
	}
	if(code == RAW1_LEFT_CTRL || code == RAW1_RIGHT_CTRL)
	{
		kbd_status |= KBD_META_CTRL;
		return -1;
	}
	if(code == RAW1_LEFT_SHIFT || code == RAW1_RIGHT_SHIFT)
	{
		kbd_status |= KBD_META_SHIFT;
		return -1;
	}
/* Scroll Lock, Num Lock, and Caps Lock set the LEDs. These keys
have on-off (toggle or XOR) action, instead of momentary action */
	if(code == RAW1_SCROLL_LOCK)
	{
		kbd_status ^= KBD_META_SCRL;
		goto LEDS;
	}
	if(code == RAW1_NUM_LOCK)
	{
		kbd_status ^= KBD_META_NUM;
		goto LEDS;
	}
	if(code == RAW1_CAPS_LOCK)
	{
		kbd_status ^= KBD_META_CAPS;
LEDS:		write_kbd(0x60, 0xED);	/* "set LEDs" command */
		temp = 0;
		if(kbd_status & KBD_META_SCRL)
			temp |= 1;
		if(kbd_status & KBD_META_NUM)
			temp |= 2;
		if(kbd_status & KBD_META_CAPS)
			temp |= 4;
		write_kbd(0x60, temp);	/* bottom 3 bits set LEDs */
		return -1;
	}
	if ((kbd_status & KBD_META_CTRL) && (kbd_status & KBD_META_ALT) && (code == RAW1_DEL)) {
		cpu_reset();							// Should invoke the kernel debugger -- for now, just reset.
	}
	if (kbd_status & KBD_META_NUM) {
		switch (code) {
			case 0x53:
				return '.';
			case 0x52:
				return '0';
			case 0x51:
				return '3';
			case 0x50:
				return '2';
			case 0x4F:
				return '1';
			case 0x4D:
				return '6';
			case 0x4C:
				return '5';
			case 0x4B:
				return '4';
			case 0x49:
				return '9';
			case 0x48:
				return '8';
			case 0x47:
				return '7';
		}
	}
/* no conversion if Alt pressed */
	if(kbd_status & KBD_META_ALT)
		return code;
/* convert A-Z[\]^_ to control chars */
	if(kbd_status & KBD_META_CTRL)
	{
		if(code >= keyboard_map_size / sizeof(keyboard_map[0]))
			return -1;
		temp = keyboard_map[code];
		if(temp >= 'a' && temp <= 'z')
			return temp - 'a';
		if(temp >= '[' && temp <= '_')
			return temp - '[' + 0x1B;
		return -1;
	}
/* convert raw scancode to ASCII */
	if(kbd_status & KBD_META_SHIFT)
	{
/* ignore invalid scan codes */
		if(code >= keyboard_shift_map_size / sizeof(keyboard_shift_map[0]))
			return -1;
		temp = keyboard_shift_map[code];
/* defective keyboard? non-US keyboard? more than 104 keys? */
		if(temp == 0)
			return -1;
/* caps lock? */
		if(kbd_status & KBD_META_CAPS)
		{
			if(temp >= 'A' && temp <= 'Z')
				temp = keyboard_map[code];
		}
	}
	else
	{
		if(code >= keyboard_map_size / sizeof(keyboard_map[0]))
			return -1;
		temp = keyboard_map[code];
		if(temp == 0)
			return -1;
		if(kbd_status & KBD_META_CAPS)
		{
			if(temp >= 'a' && temp <= 'z')
				temp = keyboard_shift_map[code];
		}
	}
	return temp;
}

unsigned char keyboard_getchar(void) {
	unsigned char scancode;
	while (empty(&g_queue))
		__asm hlt;										// Do nothing.
	
	if(deq(&g_queue, &scancode) < 0)
		return 0;
	else {
		if (scancode == 0xE0) {
			if(deq(&g_queue, &scancode) < 0)
				return 0;
			else
				return (unsigned char)set1_handle_E0(scancode);
		}
		else
			return (unsigned char)set1_scancode_to_ascii(scancode);
	}
}