// BlacklightEVO core\math.cpp -- floating point bullshit
// Copyright (c) 2013 The Cordilon Group
// Please don't steal our code. Borrowing small chunks of it is okay, as long as you give us a shout-out.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#include "global.h"
#include "core\console.h"
#include "core\math.h"

double sin(double x) {
	double retv;
	__asm {
		fld QWORD PTR [x]
		fsin
		fstp QWORD PTR [retv]
	}
	return retv;
}

double cos(double x) {
	double retv;
	__asm {
		fld QWORD PTR [x]
		fcos
		fstp QWORD PTR [retv]
	}
	return retv;
}

double tan(double x) {
	double retv;
	__asm {
		fld QWORD PTR [x]
		fptan
        ffree st(0)
        fincstp
		fst QWORD PTR [retv]
	}
	return retv;
}

double log(double x) {
	double retv;
	__asm {
		fld1
		fldl2e
		fdivrp st(1), st(0)
		fld QWORD PTR [x]
		fyl2x
		fst QWORD PTR [retv]
	}
	return retv;
}

double log10(double x) {
	double retv;
	__asm {
		fldlg2
		fld QWORD PTR [x]
		fyl2x
		fst QWORD PTR [retv]
	}
	return retv;
}

double pow(double x, double y) {
	double retv;
	long tmp;
	__asm {
		fld QWORD PTR [y]
		fld QWORD PTR [x]
		fyl2x
		fist [tmp]
		fild [tmp]
		fsub
		f2xm1
		fld1
		fadd
		fild [tmp]
		fxch
		fscale
		fst QWORD PTR [retv]
	}
	return retv;
}

int getRandomNumber(void) {
	return 4;
}