// BlacklightEVO kernel/global.c -- functions I couldn't find a better place for
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include <global.h>
#include <mm.h>

uint64_t cpu_rdtsc()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

unsigned char inb(unsigned short port) {
	unsigned char ret;
	asm volatile ("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void outb(unsigned short port, unsigned char data) {
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (data));
}

unsigned short inw(unsigned short port) {
	unsigned short ret;
	asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

void outw(unsigned short port, unsigned short data) {
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}

unsigned int ind(unsigned short port) {
	unsigned int ret;
	asm volatile ("inl %%dx, %%eax" : "=a" (ret) : "dN" (port));
	return ret;
}

void outd(unsigned short port, unsigned int data) {
	asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}

int memcmp(const void* s1, const void* s2,size_t n) {
	const unsigned char *p1 = (const unsigned char*)s1, *p2 = (const unsigned char*)s2;
	while(n--)
		if( *p1 != *p2 )
			return *p1 - *p2;
		else
			p1++,p2++;
	return 0;
}

void *memcpy(void *dest, const void *src, size_t n) {
	char *dp = (char*)dest;
	const char *sp = (const char*)src;
	while (n--)
		*dp++ = *sp++;
	return dest;
}

void *memset(void *s, int c, size_t n) {
	unsigned char* p = (unsigned char*)s;
	while(n--)
		*p++ = (unsigned char)c;
	return s;
}

size_t lfind(const char * str, const char accept) {
	size_t i = 0;
	while ( str[i] != accept) {
		i++;
	}
	return (size_t)(str) + i;
}

char *strcat(char *dest, const char *src) {
	char *ret = dest;
	while (*dest)
		dest++;
	while ((*dest++ = *src++))
		;
	return ret;
}

char *strchr(const char *s, int c) {
	while (*s != (char)c)
		if (!*s++)
			return 0;
	return (char *)s;
}

int strcmp(const char* s1, const char* s2) {
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

char *strcpy(char *dest, const char* src) {
	char *ret = dest;
	while ((*dest++ = *src++))
		;
	return ret;
}

size_t strcspn(const char *s1, const char *s2) {
	size_t ret=0;
	while(*s1)
		if(strchr(s2,*s1))
			return ret;
		else
			s1++,ret++;
	return ret;
}

char* strdup(const char* str) {
	char* ret, *tmp;
	size_t len;
 
	if (!str)
		return 0;
       
	len = 0;
	tmp = (char*)str;
	while(*(tmp++))
		len++;
	ret = (char*) malloc(len + 1);
	tmp = ret;
	while(len--)
		*(tmp++) = *(str++);
	*tmp = '\0';
 
	return ret;
}

size_t strlen(const char *s) {
	size_t i;
	for (i = 0; s[i] != '\0'; i++) ;
	return i;
}

char *strncpy(char *dest, const char *src, size_t n) {
	char *ret = dest;
	do {
		if (!n--)
			return ret;
	} while ((*dest++ = *src++));
	while (n--)
		*dest++ = 0;
	return ret;
}

char* strpbrk(const char * str, const char * accept) {
	const char *acc = accept;

	if (!*str) {
		return 0;
	}

	while (*str) {
		for (acc = accept; *acc; ++acc) {
			if (*str == *acc) {
				break;
			}
		}
		if (*acc) {
			break;
		}
		++str;
	}

	if (*acc == '\0') {
		return 0;
	}

	return (char *)str;
}

size_t strspn(const char *s1, const char *s2) {
	size_t ret=0;
	while(*s1 && strchr(s2,*s1++))
		ret++;
	return ret;    
}

char *strstr(char *s1, const char *s2)
{
    size_t n = strlen(s2);
    while(*s1)
        if(!memcmp(s1++,s2,n))
            return s1-1;
    return 0;
}

char *strtok(char *str, const char *delim)
{
	static char* p=0;
	if(str)
		p=str;
	else if(!p)
		return 0;
	str=p+strspn(p,delim);
	p=str+strcspn(str,delim);
	if(p==str)
		return p=0;
	p = *p ? *p=0,p+1 : 0;
	return str;
}
char * strtok_r(char * str, const char * delim, char ** saveptr) {
	char * token;
	if (str == NULL) {
		str = *saveptr;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		*saveptr = str;
		return 0;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == NULL) {
		*saveptr = (char *)lfind(token, '\0');
	} else {
		*str = '\0';
		*saveptr = str + 1;
	}
	return token;
}

static void strreverse(char* begin, char* end) {
	char aux;
	while (end > begin)
		aux = *end, *end-- = *begin, *begin++ = aux;
}


int abs(int val) {
	return ((val < 0) ? -val : val);
}

int atoi(const char * str) {
	unsigned int len = strlen(str);
	unsigned int out = 0;
	unsigned int i;
	unsigned int pow = 1;
	for (i = len; i > 0; --i) {
		out += (str[i-1] - 48) * pow;
		pow *= 10;
	}
	return out;
}

char* u64toa(char* buf, uint64_t val, int base, int min) {
	static char num[] = "0123456789ABCDEF";
	char* wstr=buf;
	
	// Validate base
	if (base<2 || base>16) {
		*wstr = '\0';
		return buf;
	}
	
	do {
		*wstr++ = num[(uint64_t)val%base];
		if (min > 0)
			min--;
	} while (val/=base);

	while (min > 0) {
		*wstr++ = '0';
		min--;
	}
	*wstr = '\0';

	// Reverse string
	strreverse(buf, wstr-1);
	return buf;
}

char* uitoa(char* buf, unsigned int val, int base, int min) {
	static char num[] = "0123456789ABCDEF";
	char* wstr=buf;
	
	// Validate base
	if (base<2 || base>16) {
		*wstr = '\0';
		return buf;
	}
	
	do {
		*wstr++ = num[(unsigned int)val%base];
		if (min > 0)
			min--;
	} while (val/=base);

	while (min > 0) {
		*wstr++ = '0';
		min--;
	}
	*wstr = '\0';

	// Reverse string
	strreverse(buf, wstr-1);
	return buf;
}

char* itoa(char* buf, int val, int base, int min) {
	static char num[] = "0123456789ABCDEF";
	char* wstr=buf;
	int sign;
	unsigned int uval = val;
	
	// Validate base
	if (base<2 || base>16){ *wstr='\0'; return buf; }
	
	if (uval > 0x7FFFFFFF && base == 16) {

		do {
			*wstr++ = num[(unsigned int)uval%base];
			if (min > 0)
				min--;
		} while (uval/=base);
	
		while (min > 0) {
			*wstr++ = '0';
			min--;
		}
		*wstr='\0';
	} else {
		// Take care of sign
		if ((sign=val) < 0) val = -val;
	
		// Conversion. Number is reversed.
		do {
			*wstr++ = num[(unsigned int)val%base];
			if (min > 0)
				min--;
		} while (val/=base);
	
		while (min > 0) {
			*wstr++ = '0';
			min--;
		}

		if(sign < 0 && base == 10) *wstr++='-';
		*wstr='\0';
	}
	// Reverse string
	strreverse(buf,wstr-1);
	return buf;
}
