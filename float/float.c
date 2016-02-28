#include "../natural/natural.h"

typedef struct floatn {
	bool neg; //true if neg
	int exp; //exponent
	natural *man; //mantissa
} floatn;

void floatn_print(floatn f) {
	if (f.neg) printf("- ");
	natural_print(f.man);
	printf(" E %d", f.exp);
}

void floatn_println(floatn f) {
	floatn_print(f);
	printf("\n");
}

//cool trick!
//http://stackoverflow.com/questions/15685181/how-to-get-the-sign-mantissa-and-exponent-of-a-floating-point-number
// little-endian pray!
typedef union {
	double d;
	struct {
		uint64_t  man : 52; 	//but leading one is tacit
		unsigned int exp : 11; 	//but exp is offset from -1023; we won't account for specials
		unsigned int neg: 1;  	//1 => -ve.
	} parts;
} double_split;

// CONSTRUCTORS
floatn floatn_from_natural(natural *n) {
	floatn f = {natural_new(), 0, 0};
	return f;
}

floatn floatn_new() {
	natural *n = natural_new();
	return floatn_from_natural(n);
}

// Breaking up doubles for testing

//
wide double_mantissa(double d) {
	double_split x = (double_split) d;
	wide y = (wide) x.parts.man + 1<<53; //leading 1
	y = x.parts.neg ? -y : y;
	return y;
}

int double_exponent(double d) {
	double_split x = (double_split) d;
	int y = ((int) x.parts.exp) - 1023;
	return y;
}

// for completeness
bool double_negative(double d) {
	double_split x = (double_split) d;
	bool y = ((bool) x.parts.neg);
	return y;
}

floatn floatn_from_double(double d) {
	floatn f = {double_exponent(d),
				double_negative(d),
	 			natural_from_wide(double_mantissa(d))
	};
	return f;
}

