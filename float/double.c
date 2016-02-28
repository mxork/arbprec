#include "float.c"

// TODO this stuff isn't quite necessary right now
// nifty and fun, though.

//cool trick!
//http://stackoverflow.com/questions/15685181/how-to-get-the-sign-mantissa-and-exponent-of-a-floating-point-number
// little-endian pray!
typedef struct double_split {
		uint64_t  man : 52; 	// but leading one is tacit
		uint32_t exp : 11; 	//but exp is offset from -1023; we won't account for specials
		uint32_t neg: 1;  	//1 => -ve.
	} double_split;

typedef union {
	double d;
	double_split ds;
} dbl;


// Breaking up doubles for testing

// This is terrible naming
double_split split_double(double d) {
	double_split x;
	memcpy(&x, &d, sizeof(d));
	return x;
}

//
wide double_mantissa(float d) {
	double_split x = split_double(d);
	wide y = x.man;
	return y;
}

int double_exponent(double d) {
	double_split x;
	memcpy(&x, &d, sizeof(d));
	int y = ((int) x.exp) - 1023;
	return y;
}

 /*for completeness*/
bool double_negative(double d) {
	double_split x;
	memcpy(&x, &d, sizeof(d));
	bool y = ((bool) x.neg);
	return y;
}

floatn floatn_from_double(double d) {
	floatn f = {double_exponent(d),
				double_negative(d),
				 natural_from_wide(double_mantissa(d))
	};
	return f;
}

floatn floatn_to_double() {

}

