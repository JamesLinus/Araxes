// BlacklightEVO libraries/hash/hash.c -- secure hash algorithms and HMAC
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Based on libcrypt placed in the public domain by Wei Dai and other contributors.

#ifndef __LIBRARIES__HASH__HASH_H
#define __LIBRARIES__HASH__HASH_H

#define HASH_LENGTH 20
#define BLOCK_LENGTH 64

typedef struct sha1_info {
	uint32_t buffer[BLOCK_LENGTH/4];
	uint32_t state[HASH_LENGTH/4];
	uint32_t bytecount;
	uint8_t bufferoffset;
	uint8_t key_buffer[BLOCK_LENGTH];
	uint8_t inner_hash[HASH_LENGTH];
} sha1_info;

void sha1_init(sha1_info *s);
void sha1_writebyte(sha1_info *s, uint8_t data);
void sha1_write(sha1_info *s, const char *data, size_t len);
uint8_t* sha1_result(sha1_info *s);
void sha1_hmac_init(sha1_info *s, const uint8_t* key, int key_length);
uint8_t* sha1_hmac_result(sha1_info *s);

char* hash_sha1(char* data, int length);

#endif
