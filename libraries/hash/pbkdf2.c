// BlacklightEVO libraries/hash/pbkdf2.c -- Password-Based Key Derivation Function 2
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Derived from cifra placed in the public domain by Joseph Birr-Pixton.

#include <stdint.h>
#include <stddef.h>
#include "hash.h"
#include "pbkdf2.h"
#include "base64.h"

void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
void* malloc(size_t n);
int snprintf(char* s, size_t size, const char *fmt, ...)
	__attribute__ ((format (printf, 3, 4)));
size_t strlen(const char *s);

static inline void write32_be(uint32_t v, uint8_t buf[4]) {
	*buf++ = (v >> 24) & 0xff;
	*buf++ = (v >> 16) & 0xff;
	*buf++ = (v >> 8) & 0xff;
	*buf   = v & 0xff;
}

static inline void xor_bb(uint8_t *out, const uint8_t *x, const uint8_t *y, size_t len) {
	for (size_t i = 0; i < len; i++)
		out[i] = x[i] ^ y[i];
}

static void F_sha1(const sha1_info *startctx,
                   uint32_t counter,
                   const uint8_t *salt, size_t nsalt,
                   uint32_t iterations,
                   uint8_t *out) {
	uint8_t U[CF_MAXHASH];
	size_t hashsz = 20;

	uint8_t countbuf[4];
	write32_be(counter, countbuf);

	/* First iteration:
	*   U_1 = PRF(P, S || INT_32_BE(i))
	*/
	sha1_info ctx = *startctx;
	sha1_write(&ctx, (const char*)salt, nsalt);
	sha1_write(&ctx, (const char*)countbuf, sizeof countbuf);
	memcpy(U, sha1_hmac_result(&ctx), hashsz);
	memcpy(out, U, hashsz);

	/* Subsequent iterations:
	*   U_c = PRF(P, U_{c-1})
	*/
	for (uint32_t i = 1; i < iterations; i++) {
		ctx = *startctx;
		sha1_write(&ctx, (const char*)U, hashsz);
		memcpy(U, sha1_hmac_result(&ctx), hashsz);
		xor_bb(out, out, U, hashsz);
	}
}

void pbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                      const uint8_t *salt, size_t nsalt,
                      uint32_t iterations,
                      uint8_t *out, size_t nout) {
	uint32_t counter = 1;
	uint8_t block[CF_MAXHASH];

	/* Starting point for inner loop. */
	sha1_info ctx;
	sha1_hmac_init(&ctx, pw, npw);

	while (nout) {
		F_sha1(&ctx, counter, salt, nsalt, iterations, block);

		size_t taken = MIN(nout, 20u);
		memcpy(out, block, taken);
		out += taken;
		nout -= taken;
		counter++;
	}
}

unsigned char* pbkdf2_sha1_result(char* password, char* salt, int c, int dkLen) {
	unsigned char* state = (unsigned char*) malloc(dkLen/8);
	pbkdf2_hmac_sha1((const uint8_t*)password, strlen(password), (const uint8_t*)salt, strlen(salt), c, state, dkLen/8);
	return state;
}

char* pbkdf2_sha1(char* password, char* salt, int c, int dkLen) {
	int nout = dkLen/8;
	unsigned char* result;
	char* hash = (char*)malloc(nout*2 + 1);
	if (!hash)
		return (char*)0;
	
	result = pbkdf2_sha1_result(password, salt, c, dkLen);
	
	for (int i = 0; i < nout; i++) {
		snprintf(hash+i*2, 2, "%02x", result[i]);
	}
	hash[nout*2] = '\0';
	return hash;
}

char* pwstring_pbkdf2_sha1(char* password, char* salt, int c, int dkLen) {
	size_t bufsize = dkLen/4 + dkLen/8 + 1;
	char* out = (char*)malloc(bufsize);
	if (!out)
		return (char*)0;
	char* hash = pbkdf2_sha1(password, salt, c, dkLen);
	base64_encode(hash, strlen(hash), out, bufsize);
	
	size_t saltbufsize = strlen(salt) + strlen(salt)/2 + 1;
	char* salt64 = (char*)malloc(saltbufsize);
	if (!salt64)
		return (char*)0;
	base64_encode(salt, strlen(salt), salt64, saltbufsize);
	
	char* result = (char*)malloc(bufsize + saltbufsize + 17);	// A complete ballpark for how long we need, honestly.
	if (!result)
		return (char*)0;
	
	snprintf(result, bufsize + saltbufsize + 17, "1a:%d:%d:%s:%s", c, dkLen, salt64, out);
	free(out);
	free(salt64);
	return result;
}