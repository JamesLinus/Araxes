// BlacklightEVO libraries/hash/sha1.c -- secure hash algorithms and HMAC
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Based on libcrypt placed in the public domain by Wei Dai and other contributors.

#include <stdint.h>
#include <stddef.h>
#include "hash.h"

void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
void* malloc(size_t n);
int snprintf(char* s, size_t size, const char *fmt, ...)
	__attribute__ ((format (printf, 3, 4)));

#define SHA1_K0  0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

void sha1_init(sha1_info *s) {
	s->state[0] = 0x67452301;
	s->state[1] = 0xefcdab89;
	s->state[2] = 0x98badcfe;
	s->state[3] = 0x10325476;
	s->state[4] = 0xc3d2e1f0;
	s->bytecount = 0;
	s->bufferoffset = 0;
}

uint32_t sha1_rol32(uint32_t number, uint8_t bits) {
	return ((number << bits) | (number >> (32-bits)));
}

void sha1_hashblock(sha1_info *s) {
	uint8_t i;
	uint32_t a,b,c,d,e,t;

	a=s->state[0];
	b=s->state[1];
	c=s->state[2];
	d=s->state[3];
	e=s->state[4];
	for (i=0; i<80; i++) {
		if (i>=16) {
			t = s->buffer[(i+13)&15] ^ s->buffer[(i+8)&15] ^ s->buffer[(i+2)&15] ^ s->buffer[i&15];
			s->buffer[i&15] = sha1_rol32(t,1);
		}
		if (i<20) {
			t = (d ^ (b & (c ^ d))) + SHA1_K0;
		} else if (i<40) {
			t = (b ^ c ^ d) + SHA1_K20;
		} else if (i<60) {
			t = ((b & c) | (d & (b | c))) + SHA1_K40;
		} else {
			t = (b ^ c ^ d) + SHA1_K60;
		}
		t+=sha1_rol32(a,5) + e + s->buffer[i&15];
		e=d;
		d=c;
		c=sha1_rol32(b,30);
		b=a;
		a=t;
	}
	s->state[0] += a;
	s->state[1] += b;
	s->state[2] += c;
	s->state[3] += d;
	s->state[4] += e;
}

void sha1_adduncounted(sha1_info *s, uint8_t data) {
	uint8_t * const b = (uint8_t*) s->buffer;
	b[s->bufferoffset ^ 3] = data;
	s->bufferoffset++;
	if (s->bufferoffset == BLOCK_LENGTH) {
		sha1_hashblock(s);
		s->bufferoffset = 0;
	}
}

void sha1_writebyte(sha1_info *s, uint8_t data) {
	++s->bytecount;
	sha1_adduncounted(s, data);
}

void sha1_write(sha1_info *s, const char *data, size_t len) {
	for (;len--;) sha1_writebyte(s, (uint8_t) *data++);
}

void sha1_pad(sha1_info *s) {
	// Implement SHA-1 padding (fips180-2 Â§5.1.1)

	// Pad with 0x80 followed by 0x00 until the end of the block
	sha1_adduncounted(s, 0x80);
	while (s->bufferoffset != 56) sha1_adduncounted(s, 0x00);

	// Append length in the last 8 bytes
	sha1_adduncounted(s, 0); // We're only using 32 bit lengths
	sha1_adduncounted(s, 0); // But SHA-1 supports 64 bit lengths
	sha1_adduncounted(s, 0); // So zero pad the top bits
	sha1_adduncounted(s, s->bytecount >> 29); // Shifting to multiply by 8
	sha1_adduncounted(s, s->bytecount >> 21); // as SHA-1 supports bitstreams as well as
	sha1_adduncounted(s, s->bytecount >> 13); // byte.
	sha1_adduncounted(s, s->bytecount >> 5);
	sha1_adduncounted(s, s->bytecount << 3);
}

uint8_t* sha1_result(sha1_info *s) {
	// Pad to complete the last block
	sha1_pad(s);
	
	// Swap byte order back
	int i;
	for (i=0; i<5; i++) {
		s->state[i]=
			  (((s->state[i])<<24)& 0xff000000)
			| (((s->state[i])<<8) & 0x00ff0000)
			| (((s->state[i])>>8) & 0x0000ff00)
			| (((s->state[i])>>24)& 0x000000ff);
	}

	// Return pointer to hash (20 characters)
	return (uint8_t*) s->state;
}

#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5c

void sha1_hmac_init(sha1_info *s, const uint8_t* key, int key_length) {
	uint8_t i;
	memset(s->key_buffer, 0, BLOCK_LENGTH);
	if (key_length > BLOCK_LENGTH) {
		// Hash long keys
		sha1_init(s);
		for (;key_length--;) sha1_writebyte(s, *key++);
		memcpy(s->key_buffer, sha1_result(s), HASH_LENGTH);
	} else {
		// Block length keys are used as is
		memcpy(s->key_buffer, key, key_length);
	}
	// Start inner hash
	sha1_init(s);
	for (i=0; i<BLOCK_LENGTH; i++) {
		sha1_writebyte(s, s->key_buffer[i] ^ HMAC_IPAD);
	}
}

uint8_t* sha1_hmac_result(sha1_info *s) {
	uint8_t i;
	// Complete inner hash
	memcpy(s->inner_hash,sha1_result(s),HASH_LENGTH);
	// Calculate outer hash
	sha1_init(s);
	for (i=0; i<BLOCK_LENGTH; i++) sha1_writebyte(s, s->key_buffer[i] ^ HMAC_OPAD);
	for (i=0; i<HASH_LENGTH; i++) sha1_writebyte(s, s->inner_hash[i]);
	return sha1_result(s);
}

char* hash_sha1(char* data, int length) {
	unsigned char* result;
	char* hash = (char*)malloc(41);
	if (!hash)
		return (char*)0;
	
	sha1_info sha1_context;
	sha1_init(&sha1_context);
	sha1_write(&sha1_context, data, length);
	result = sha1_result(&sha1_context);
	
	for (int i = 0; i < 20; i++) {
		snprintf(hash+i*2, 3, "%02x", result[i]);
	}
	hash[41] = '\0';
	return hash;
}

