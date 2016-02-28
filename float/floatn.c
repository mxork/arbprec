#include "../natural/natural.h"
#include "floatn.h"
#include <math.h>

#define POS 0
#define NEG 1

typedef struct floatn {
	bool sgn; // POS and NEG
	int exp; //exponent
	natural *man; //mantissa
} floatn;

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
// could also use n->c
int natural_zeroes_on_right(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + MAX_DIGITS;

	while(p < end && *p == 0) p++;

	return (int) (p - n->digits); 
}

bool natural_is_zero(natural *n) {
	return natural_zeroes_on_right(n) == MAX_DIGITS;
}

// correct exp by number of zeroes, return
// amount of shift
int floatn_pop_zeroes_ip(floatn *f) {
	int nzeroes = natural_zeroes_on_right(f->man);
	if (nzeroes == MAX_DIGITS) {
		f->exp = 0;
		return MAX_DIGITS;
	}
	natural_shift_ip(f->man, -nzeroes);
	f->exp += nzeroes;
	return nzeroes;
}

// CONSTRUCTORS
floatn floatn_from_natural(natural *n) {
	natural *m = natural_new();
	memcpy(m, n, sizeof(natural));
	floatn f = {POS, 0, m};
	floatn_pop_zeroes_ip(&f);
	return f;
}

// as above, but takes over ownership of n: ie, assumes
// not use elsewhere
floatn floatn_devour_natural(natural *n) {
	floatn f = {POS, 0, n};
	floatn_pop_zeroes_ip(&f);
	return f;
}


floatn floatn_new() {
	natural *n = natural_new();
	return floatn_from_natural(n);
}

wide floatn_to_wide(floatn f) {
	wide x = natural_to_wide(f.man);
	x *= (int64_t) pow(BASE, f.exp);
	int sign = f.sgn ? -1 : 1;
	return sign*x;
}

// f and g are unmodified, rp is hence pointer
void floatn_add_into(floatn f, floatn g, floatn *rp) {
	int diff = f.exp - g.exp;
	natural tmp = {};
	natural *fm = f.man, *gm = g.man;
	
	// careful here - alignment
	// one of the ptrs gets subbed out so we can
	// work on it without overwriting
	if (diff > 0) {
		fm = &tmp;
		memcpy(fm, f.man, sizeof(natural));
		natural_shift_ip(fm, diff);
	} else if (diff < 0) {
		gm = &tmp;
		memcpy(gm, g.man, sizeof(natural));
		natural_shift_ip(gm, -diff);
	}

	// so f and g are aligned with the lower of the two
	// note we can't just switch them and assume, because
	// we want to be able todo thsi inplace at some point

	// here on be some messy shit cause of sign
	// handling differences
	
	// we have what looks like copy-pasta branching, but
	// again, this is to preserve identity of args
	// bottom line, we turn addition into subtraction
	// if we have to, and case analyze sign of the difference
	if (f.sgn == g.sgn) {
		natural_add_into(fm, gm, rp->man);
		rp->sgn = f.sgn;
	} else {
		if ( f.sgn == NEG ) {
			if ( natural_gt(gm, fm) ) {
				natural_subtract_into(gm, fm, rp->man);
				rp->sgn = POS;
			} else {
				natural_subtract_into(fm, gm, rp->man);
				rp->sgn = NEG;
			}
		} else if ( g.sgn == NEG) {
			if ( natural_gt(fm, gm) ) {
				natural_subtract_into(fm, gm, rp->man);
				rp->sgn = POS;
			} else {
				natural_subtract_into(gm, fm, rp->man);
				rp->sgn = NEG;
			}
		}
	}

	// result mantissa and sign correct as of here
	// set exp to least and correct for zeroes
	rp->exp = f.exp < g.exp ? f.exp : g.exp;
	assert(rp->exp <= f.exp && rp->exp <= g.exp); // this is so unecessary, but better safe than sorry

	floatn_pop_zeroes_ip(rp);
}
