// BlacklightEVO libraries/hash/hash.c -- secure hash algorithms and HMAC
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
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
		snprintf(hash+i*2, 2, "%02x", result[i]);
	}
	hash[41] = '\0';
	return hash;
}

/* self-test */

#if SHA1TEST
#include <stdio.h>

uint8_t hmacKey1[]={
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f
};
uint8_t hmacKey2[]={
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43
};
uint8_t hmacKey3[]={
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
	0xb0,0xb1,0xb2,0xb3
};
uint8_t hmacKey4[]={
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
	0xa0
};

void printHash(uint8_t* hash) {
	int i;
	for (i=0; i<20; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");
}


int main (int argc, char **argv) {
	uint32_t a;
	sha1nfo s;

	// SHA tests
	printf("Test: FIPS 180-2 C.1 and RFC3174 7.3 TEST1\n");
	printf("Expect:a9993e364706816aba3e25717850c26c9cd0d89d\n");
	printf("Result:");
	sha1_init(&s);
	sha1_write(&s, "abc", 3);
	printHash(sha1_result(&s));
	printf("\n\n");

	printf("Test: FIPS 180-2 C.2 and RFC3174 7.3 TEST2\n");
	printf("Expect:84983e441c3bd26ebaae4aa1f95129e5e54670f1\n");
	printf("Result:");
	sha1_init(&s);
	sha1_write(&s, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
	printHash(sha1_result(&s));
	printf("\n\n");

	printf("Test: RFC3174 7.3 TEST4\n");
	printf("Expect:dea356a2cddd90c7a7ecedc5ebb563934f460452\n");
	printf("Result:");
	sha1_init(&s);
	for (a=0; a<80; a++) sha1_write(&s, "01234567", 8);
	printHash(sha1_result(&s));
	printf("\n\n");

	// HMAC tests
	printf("Test: FIPS 198a A.1\n");
	printf("Expect:4f4ca3d5d68ba7cc0a1208c9c61e9c5da0403c0a\n");
	printf("Result:");
	sha1_initHmac(&s, hmacKey1, 64);
	sha1_write(&s, "Sample #1",9);
	printHash(sha1_resultHmac(&s));
	printf("\n\n");

	printf("Test: FIPS 198a A.2\n");
	printf("Expect:0922d3405faa3d194f82a45830737d5cc6c75d24\n");
	printf("Result:");
	sha1_initHmac(&s, hmacKey2, 20);
	sha1_write(&s, "Sample #2", 9);
	printHash(sha1_resultHmac(&s));
	printf("\n\n");

	printf("Test: FIPS 198a A.3\n");
	printf("Expect:bcf41eab8bb2d802f3d05caf7cb092ecf8d1a3aa\n");
	printf("Result:");
	sha1_initHmac(&s, hmacKey3,100);
	sha1_write(&s, "Sample #3", 9);
	printHash(sha1_resultHmac(&s));
	printf("\n\n");

	printf("Test: FIPS 198a A.4\n");
	printf("Expect:9ea886efe268dbecce420c7524df32e0751a2a26\n");
	printf("Result:");
	sha1_initHmac(&s, hmacKey4,49);
	sha1_write(&s, "Sample #4", 9);
	printHash(sha1_resultHmac(&s));
	printf("\n\n");

	// Long tests
	printf("Test: FIPS 180-2 C.3 and RFC3174 7.3 TEST3\n");
	printf("Expect:34aa973cd4c4daa4f61eeb2bdbad27316534016f\n");
	printf("Result:");
	sha1_init(&s);
	for (a=0; a<1000000; a++) sha1_writebyte(&s, 'a');
	printHash(sha1_result(&s));

	return 0;
}
#endif /* self-test */
