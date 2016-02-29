#include "../float/floatn.h"

void floatn_divide_into_setprecision(floatn f, floatn g, floatn *r, int prec) {
	natural tmpf = {};//, tmpg = {};
	natural *fm = &tmpf, *gm = g.man;
	memcpy(fm, f.man, sizeof(natural)); 

	int ndigits = prec; // output will have ndigits
	int s = ndigits + 1; // 1==guard digits. 

	int shift = s - (fm->c - gm->c); // ie if g is longer, need more shift on f
	natural_shift_ip(fm, shift); // hereon fm->c may lie
	natural_divide_into(fm, gm, r->man); 

	r->exp = f.exp - g.exp - (shift);
	floatn_round_ip(r, ndigits);
	r->sgn = f.sgn ^ g.sgn;
};

void floatn_invert_into(floatn f, floatn *r) {
	natural tmp = {}; 
	natural_from_wide_into(1, &tmp);
	floatn one = {POS, 0, &tmp};

	floatn_divide_into_setprecision(one, f, r, 102);
}

void arctan_into(floatn f, floatn *r) {
	floatn z = floatn_new();
	double x = floatn_to_double(f);
	if (x > 1) {
		floatn_invert_into(f, &z);
	}

	// so, here z is argument
	floatn term = floatn_new();	
	int n = 0;
	while (n < 400) {
		floatn_add_into(z, term, &z); // dbl check ip works
		n++;
	}

}
