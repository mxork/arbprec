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

// it's like pi, but longer
floatn pii = {};

// TODO tests
void floatn_sqrt_into(floatn f, floatn *r) {
	assert( f.sgn = POS );

	// we need some scratch space
	natural tmpn = {}; 
	floatn tmp = {POS, 0, &tmpn};
	
	// we need some scratch space
	natural tmptwo = {}; natural_from_wide_into(2, &tmptwo);
	floatn two = {POS, 0, &tmptwo};

	// intial guess, x0 = f with exponent in half
	natural tmpxn = {}; natural_from_wide_into(1, &tmpxn);
	floatn xn = {POS, f.exp/2, &tmpxn};
	memcpy(xn.man, f.man, sizeof(natural));

	// ie, xn+1 = 1/2(xn + f/xn)
	// TODO set precision??
	while (true) {
		floatn_divide_into_setprecision(f, xn, &tmp, MAX_DIGITS/2);
		floatn_add_into(xn, tmp, &xn);
		floatn_divide_into_setprecision(xn, two, &xn, MAX_DIGITS/2);
	}

	memcpy(r->man, xn.man, sizeof(natural));
	r->exp = xn.exp;
}

// tidy this up
// want arctan(x) = 2arctan( x / 1+sqrt(1+x^2) )
void floatn_half_angle_op() {
	natural tmpn = {}; natural_from_wide_into(1, &tmpn);
	floatn one = {POS, 0, &tmpn};


	
}

void arctan_into(floatn f, floatn *r) {
	double fd = floatn_to_double(f);
	assert(fd > 0 && fd <= 10);
	assert(f.man->c <= 101);

	floatn z = floatn_new();
	// maeke z = f

	// take the obvious  range reduction,
	// and make a note to correct later
	// TODO 102 digits of PI
	bool inverted = false;
	if (fd > 1) {
		floatn_invert_into(f, &z);
		inverted = true;
	}

	// now, perform the maybe less obvious range 
	// reduction. Could branch, but it's cheap
	// enough that we may as well always do it
	// TODO implement sqrt
	floatn_sqrt_into(z, &z);

	// so, here z is argument. Taylor
	floatn sum = floatn_new();	
	floatn term = floatn_new();	
	int n = 0;
	while (n < 400) {
		floatn_add_into(z, term, &z); // dbl check ip works
		n++;
	}
	
	// inverse range reduction -  half-angle
	


	// inverse range reduction - reciprocal
	if (inverted) {
		sum.sgn = NEG;
		floatn_add_into(pii, sum, sum); //TODO does this work
	}

}
