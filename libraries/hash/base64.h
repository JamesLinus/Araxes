// BlacklightEVO libraries/hash/base64.h -- Base64 encode/decode
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Based on libcrypt placed in the public domain by Wei Dai and other contributors.

#ifndef __LIBRARIES__HASH__BASE64_H
#define __LIBRARIES__HASH__BASE64_H

#include <stdint.h>
#include <stddef.h>

int base64_encode(const void* data_buf, size_t datalength, char* result, size_t outsize);
int base64_decode(char *in, size_t length, unsigned char *out, size_t outsize);

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

#endif	// __LIBRARIES__HASH__BASE64_H
