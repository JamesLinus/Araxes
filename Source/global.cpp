// BlacklightEVO global.cpp -- global functions and macros that are useful
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\mm.h"
//#include <float.h>

double log10(double x);
double pow(double x, double y);

static const double powers_of_10[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                                      10000000, 100000000, 1000000000};

extern "C" {
	int _fltused = 1;
}

extern "C" long _ftol2_sse() {
	long a;
	short b;
	_asm {
		fstcw [b]								// xxx - retardedly slow, gotta find a better way to do this
		or [b], 0x0C00
		fldcw [b]
		fistp [a]
		fstcw [b]
		and [b], 0xF3FF
		fldcw [b]
	}
	return a;
}

extern "C" __declspec(naked) void __allshl() {			// Handles << for 64-bit integers
	__asm {
		cmp cl, 64
		jb not_too_big
		xor eax, eax
		xor edx, edx
		ret

not_too_big:
		cmp cl, 32
		jae more_than_32
		shld edx, eax, cl
		shl eax, cl
		ret

more_than_32:
		mov edx, eax
		xor eax, eax
		and cl, 31
		shl edx, cl
		ret
	}
}

extern "C" __int64 __stdcall __allmul(__int64 multiplicand, __int64 multiplier) {
	__asm {
		mov eax, DWORD PTR multiplier[4]
		mov ecx, DWORD PTR multiplicand[4]
		or  ecx, eax
		mov ecx, DWORD PTR multiplicand
		jnz notzero

		mov eax, DWORD PTR multiplier
		mul ecx
		jmp done

	notzero:
		mov eax, DWORD PTR multiplier[4]
		mul ecx
		mov esi, eax

		mov eax, DWORD PTR multiplier 
		mul DWORD PTR multiplicand[4]
		add esi, eax


		mov eax, DWORD PTR multiplier
		mul ecx
		add edx, esi

	done:
	}
}

unsigned long long cpu_rdtsc() {
	__asm {
		rdtsc
		xchg eax, edx
	}
}

int memcmp(const void* s1, const void* s2,size_t n) {
	const unsigned char *p1 = (const unsigned char*)s1, *p2 = (const unsigned char*)s2;
	while(n--)
		if( *p1 != *p2 )
			return *p1 - *p2;
		else
			*p1++,*p2++;
	return 0;
}

#pragma optimize("", on)
void *memcpy(void *dest, const void *src, size_t n) {
	char *dp = (char*)dest;
	const char *sp = (const char*)src;
	while (n--)
		*dp++ = *sp++;
	return dest;
}
#pragma optimize("", off)

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
	while (*dest++ = *src++)
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
	while (*dest++ = *src++)
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
	} while (*dest++ = *src++);
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

char* itoa(int val, int base) {
	static char buf[33] = {0};
	return itoa(buf, val, base, 1);
}


char* itoa(int val, int base, int min) {
	static char buf[33] = {0};
	return itoa(buf, val, base, min);
}

char* itoa(char* buf, int val, int base) {
	return itoa(buf, val, base, 1);
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

/*char* lltoa(long long val, int base, int min) {
	static char buf[65] = {0};
	return lltoa(buf, val, base, min);
}

char* lltoa(char* buf, long long val, int base, int min) {
	static char num[] = "0123456789ABCDEF";
	char* wstr=buf;
	long long sign;
	unsigned long long uval = val;
	
	// Validate base
	if (base<2 || base>16){ *wstr='\0'; return buf; }
	
	if (uval > 0x7FFFFFFFFFFFFFFF && base == 16) {

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
			*wstr++ = num[(unsigned long long)val%base];
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
}*/

#if 0
char* itoa(char* buf, int val, int base, int min) {
	// check that the base if valid
	if (base < 2 || base > 16) { *buf = 0; return buf; }
	
	char* out = buf;
	int quotient = val;
	
	do {
		*out = "0123456789ABCDEF"[abs(quotient % base)];
		++out;
		quotient /= base;
		//if (min > 0)
			//min--;
	} while ( quotient );
	
	//while (min > 0) {
	//	*out = '0';
	//	++out;
	//	min--;
	//}
	
	// Only apply negative sign for base 10
	if ( val < 0 && base == 10) *out++ = '-';

	strreverse(buf, out);
	*out = 0;
	return buf;
}
#endif
#if 0
char* itoa(char* buf, int val, int base, int min) {
	int i = 30;
	memset(buf, '0', 31);
	for(; val && i ; --i, val /= base) {
		buf[i] = "0123456789ABCDEF"[val % base];
		if (min > 0)
			min--;
	}
	while (min > 0) {
		buf[--i] = '0';
		min--;
	}
	return &buf[i+1];
}
#endif

#if 1
char* dtoa(double num) {
	static char buf[32] = {0};
	return dtoa(buf, num, 8);
}

char *dtoa(double num, int precision) {
	static char buf[32] = {0};
	return dtoa(buf, num, precision);
}

char* dtoa(char* buf, double num) {
	return dtoa(buf, num, 8);
}

char* dtoa(char* buf, double num, int precision)
{
    /* Hacky test for NaN
     * under -fast-math this won't work, but then you also won't
     * have correct nan values anyways.  The alternative is
     * to link with libmath (bad) or hack IEEE double bits (bad)
     */
    if (! (num == num)) {
        buf[0] = 'n'; buf[1] = 'a'; buf[2] = 'n'; buf[3] = '\0';
        return buf;
    }
    /* if input is larger than thres_max, revert to exponential */
    const double thres_max = (double)(0x7FFFFFFF);

    double diff = 0.0;
    char* wstr = buf;

    if (precision < 0) {
        precision = 0;
    } else if (precision > 9) {
        /* precision of >= 10 can lead to overflow errors */
        precision = 9;
    }


    /* we'll work in positive values and deal with the
       negative sign issue later */
    int neg = 0;
    if (num < 0) {
        neg = 1;
        num = -num;
    }


    int whole = (int) num;
    double tmp = (num - whole) * powers_of_10[precision];
    unsigned int frac = (unsigned int)(tmp);
    diff = tmp - frac;

    if (diff > 0.5) {
        ++frac;
        /* handle rollover, e.g.  case 0.99 with prec 1 is 1.0  */
        if (frac >= powers_of_10[precision]) {
            frac = 0;
            ++whole;
        }
    } else if (diff == 0.5 && ((frac == 0) || (frac & 1))) {
        /* if halfway, round up if odd, OR
           if last digit is 0.  That last part is strange */
        ++frac;
    }

    /* for very large numbers switch back to native sprintf for exponentials.
       anyone want to write code to replace this? */
    /*
      normal printf behavior is to print EVERY whole number digit
      which can be 100s of characters overflowing your buffers == bad
    */
    if (num > thres_max) {
        buf[0] = 'n'; buf[1] = 'a'; buf[2] = 'n'; buf[3] = '\0';
        return buf;
    }

    if (precision == 0) {
        diff = num - whole;
        if (diff > 0.5) {
            /* greater than 0.5, round up, e.g. 1.6 -> 2 */
            ++whole;
        } else if (diff == 0.5 && (whole & 1)) {
            /* exactly 0.5 and ODD, then round up */
            /* 1.5 -> 2, but 2.5 -> 2 */
            ++whole;
        }
    } else {
        int count = precision;
        // now do fractional part, as an unsigned number
        do {
            --count;
            *wstr++ = (char)(48 + (frac % 10));
        } while (frac /= 10);
        // add extra 0s
        while (count-- > 0) *wstr++ = '0';
        // add decimal
        *wstr++ = '.';
    }

    // do whole part
    // Take care of sign
    // Conversion. Number is reversed.
    do *wstr++ = (char)(48 + (whole % 10)); while (whole /= 10);
    if (neg) {
        *wstr++ = '-';
    }
    *wstr='\0';
    strreverse(buf, wstr-1);
    return buf;
}

/*
char *ftoa(char* buf, float num, int precision)
{
   int whole_part = num;
   int digit = 0, reminder =0;
   int log_value = log10(num), index = log_value;
   long wt =0;

   // String containg result
   char* str = buf;

   //Initilise stirng to zero
   memset(str, 0, 32);

   //Extract the whole part from float num
   for(int  i = 1 ; i < log_value + 2 ; i++)
   {
       wt  =  pow(10.0,i);
       reminder = whole_part  %  wt;
       digit = (reminder - digit) / (wt/10);

       //Store digit in string
       str[index--] = digit + 48;              // ASCII value of digit  = digit + 48
       if (index == -1)
          break;    
   }

    index = log_value + 1;
    str[index] = '.';

   float fraction_part  = num - whole_part;
   float tmp1 = fraction_part,  tmp =0;

   //Extract the fraction part from  num
   for( int i= 1; i < precision; i++)
   {
      wt =10; 
      tmp  = tmp1 * wt;
      digit = tmp;

      //Store digit in string
      str[++index] = digit +48;           // ASCII value of digit  = digit + 48
      tmp1 = tmp - digit;
   }    

   return str;
}*/
#endif