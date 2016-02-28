#include "floatn.h"

void floatn_add_into(floatn f, floatn g, floatn r) {
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
		natural_shift_ip(gm, diff);
	}

	// so f and g are aligned with the lower of the two
	// note we can't just switch them and assume, because
	// we want to be able todo thsi inplace at some point

	// here on be some messy shit cause of sign
	// handling differences
	
	natural_add_into(fm, gm, r.man);

	// we have what looks like copy-pasta branching, but
	// again, this is to preserve identity of args
	// bottom line, we turn addition into subtraction
	// if we have to, and case analyze sign of the difference
	if (f.sgn == g.sgn) {
		natural_add_into(fm, gm, r.man);
		r.sgn = f.sgn;
	} else {
		if ( f.sgn == NEG ) {
			if ( natural_gt(gm, fm) ) {
				natural_subtract_into(gm, fm, r.man);
				r.sgn = POS;
			} else {
				natural_subtract_into(fm, gm, r.man);
				r.sgn = NEG;
			}
		} else if ( g.sgn == NEG) {
			if ( natural_gt(fm, gm) ) {
				natural_subtract_into(fm, gm, r.man);
				r.sgn = POS;
			} else {
				natural_subtract_into(gm, fm, r.man);
				r.sgn = NEG;
			}
		}
	}

	// result mantissa and sign correct as of here
	// set exp to least and correct for zeroes
	r.exp = f.exp < g.exp ? f.exp : g.exp;
	assert(r.exp <= f.exp && r.exp <= g.exp); // this is so unecessary, but better safe than sorry

	floatn_pop_zeroes_ip(r);
}
