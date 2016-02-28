#include "floatn.h"

void floatn_add_into(floatn f, floatn g, floatn h) {
	int diff = f.exp - g.exp;
	int min_exp = f.exp < g.exp ? f.exp : g.exp;

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
	natural_add_into(fm, gm, h.man);

}
