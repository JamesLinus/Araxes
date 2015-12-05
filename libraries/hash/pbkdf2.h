// BlacklightEVO libraries/hash/pbkdf2.h -- Password-Based Key Derivation Function 2
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

// Derived from cifra placed in the public domain by Joseph Birr-Pixton.

#ifndef __LIBRARIES__HASH__PBKDF2_H
#define __LIBRARIES__HASH__PBKDF2_H

#define CF_MAXHASH 64
#define MIN(x, y) \
	({ typeof (x) __x = (x); \
	   typeof (y) __y = (y); \
	   __x < __y ? __x : __y; })

unsigned char* pbkdf2_sha1_result(char* password, char* salt, int c, int dkLen);
char* pbkdf2_sha1(char* password, char* salt, int c, int dkLen);

char* pwstring_pbkdf2_sha1(char* password, char* salt, int c, int dkLen);

#endif	// __LIBRARIES__HASH__PBKDF2_H