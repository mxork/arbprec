#include "floatn.h"

void floatn_print(floatn f) {
	if (f.sgn) printf("- ");
	natural_print(f.man);
	printf(" E %d", f.exp);
}

void floatn_println(floatn f) {
	floatn_print(f);
	printf("\n");
}

// misc
int natural_zeroes_on_right(natural *n) {
	slim *p = n->digits;

	while(*p == 0) p++;

	// off by one
	// [0 0 0] -> [0 1 2] -> 2-0 != 3
	return (int) (p - n->digits +1); 
}

// CONSTRUCTORS
floatn floatn_from_natural(natural *n) {
	int exp = natural_zeroes_on_right(n);
	floatn f = {POS, exp, natural_shift(n, exp)};
	return f;
}

floatn floatn_new() {
	natural *n = natural_new();
	return floatn_from_natural(n);
}

