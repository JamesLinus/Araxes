// BlacklightEVO libraries/hash/base64.c -- Base64 encode/decode
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <stdint.h>
#include <stddef.h>
#include "base64.h"

static const unsigned char d[] = {
	66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
	54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
	10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
	29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
	66,66,66,66,66,66
};

int base64_encode(const void* data_buf, size_t datalength, char* result, size_t outsize) {
	const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const uint8_t *data = (const uint8_t *)data_buf;
	size_t index = 0;
	size_t x = 0;
	uint32_t n = 0;
	int padCount = datalength % 3;
	uint8_t n0 = 0, n1 = 0, n2 = 0, n3 = 0;

	for (x = 0; x < datalength; x += 3) {
		n = ((uint32_t)data[x]) << 16;
		
		if ((x+1) < datalength)
			n += ((uint32_t)data[x+1]) << 8;
		
		if ((x+2) < datalength)
			n += data[x+2];

		n0 = (uint8_t)(n >> 18) & 63;
		n1 = (uint8_t)(n >> 12) & 63;
		n2 = (uint8_t)(n >> 6) & 63;
		n3 = (uint8_t)n & 63;
		
		if (index >= outsize)
			return -1;
		result[index++] = base64chars[n0];
		
		if (index >= outsize)
			return -1;
		result[index++] = base64chars[n1];

		if ((x+1) < datalength) {
			if (index >= outsize)
				return -1;
			result[index++] = base64chars[n2];
		}

		if ((x+2) < datalength) {
			if (index >= outsize)
				return -1;
			result[index++] = base64chars[n3];
		}
	}  

	if (padCount > 0) { 
		for (; padCount < 3; padCount++) 
		{ 
			if (index >= outsize)
				return -1;
			result[index++] = '=';
		} 
	}
	if (index >= outsize)
		return -1;
	result[index] = 0;
	return 0;
}

int base64_decode(char *in, size_t length, unsigned char *out, size_t outsize) { 
	char *end = in + length;
	char iter = 0;
	size_t buf = 0, len = 0;
	
	while (in < end) {
		unsigned char c = d[*in++];
		
		switch (c) {
		case WHITESPACE:
			continue;
		case INVALID:
			return -1;
		case EQUALS:
			in = end;
			continue;
		default:
			buf = buf << 6 | c;
			iter++;
			
			if (iter == 4) {
				if ((len += 3) > outsize)
					return -1;
				*(out++) = (buf >> 16) & 255;
				*(out++) = (buf >> 8) & 255;
				*(out++) = buf & 255;
				buf = 0; iter = 0;

			}   
		}
	}
   
	if (iter == 3) {
		if ((len += 2) > outsize)
			return -1;
		*(out++) = (buf >> 10) & 255;
		*(out++) = (buf >> 2) & 255;
	}
	else if (iter == 2) {
		if (++len > outsize)
			return -1;
		*(out++) = (buf >> 4) & 255;
	}
	
	return 0;
}