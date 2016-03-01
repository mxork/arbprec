#include "../float/floatn.h"

// it's like pi2, but longer
// base 10
natural pii2man = {253, {2,7,5,4,5,9,0,0,6,5,3,6,2,8,5,1,9,3,3,9,8,6,1,1,4,2,8,7,3,2,4,6,0,3,2,7,6,6,9,2,3,8,7,8,4,5,0,4,4,1,2,2,8,9,0,9,1,5,6,4,7,7,4,4,7,4,1,1,3,2,2,8,9,7,7,2,5,5,0,6,2,9,6,9,0,5,3,1,5,0,2,4,1,5,2,7,8,5,5,0,4,2,4,6,0,4,0,7,9,7,6,2,6,8,5,1,1,1,9,2,5,7,7,4,0,3,2,2,9,6,4,5,3,2,3,3,5,1,1,4,6,6,5,2,3,4,0,4,7,0,1,9,9,3,3,5,8,5,0,1,7,6,2,1,4,7,1,0,4,1,3,9,9,4,4,0,1,3,4,1,3,0,2,8,0,9,3,5,1,6,9,2,2,7,4,7,8,4,0,1,9,2,5,5,7,8,6,9,9,6,4,8,5,8,9,0,2,4,4,1,5,7,9,3,6,1,9,6,1,2,3,1,3,2,9,1,6,6,9,8,4,9,7,6,2,3,6,9,7,0,7,5,1}}; 

/*1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533991074043256641153323546922304775291115862679704064240558725142051350969260552779822311474477465190982214405487832966723064237824116893391582635600954572*/

floatn pii2 = {POS, -252, &pii2man};

void floatn_divide_into_setprecision2(floatn f, floatn g, floatn *r, int prec) {
	natural tmpf = {};// tmpg = {};
	natural *fm = &tmpf; natural *gm = g.man;
	memcpy(fm, f.man, sizeof(natural)); 
	/*memcpy(gm, g.man, sizeof(natural)); */

	int ndigits = prec; // output will have ndigits
	int s = ndigits + 1; // 1==guard digits. 

	int shift = s - (fm->c - gm->c); // ie if g is longer, need more shift on f

	natural_shift_ip(fm, shift); // hereon fm->c may lie

	natural_divide_into(fm, gm, r->man); 

	r->exp = f.exp - g.exp - (shift);
	floatn_round_ip(r, ndigits);
	r->sgn = f.sgn ^ g.sgn;
};

// hacky
void floatn_divide_into_setprecision(floatn f, floatn g, floatn *r, int prec) {
	floatn g_rounded = floatn_new();
	g_rounded.exp = g.exp; g_rounded.sgn = g.sgn;
	memcpy(g_rounded.man, g.man, sizeof(natural));
	floatn_round_ip(&g_rounded, prec+1);
	floatn_divide_into_setprecision2(f, g_rounded, r, prec);
	free(g_rounded.man);
};

void floatn_invert_into(floatn f, floatn *r) {
	natural tmp = {}; 
	natural_from_wide_into(1, &tmp);
	floatn one = {POS, 0, &tmp};

	floatn_divide_into_setprecision(one, f, r, MAX_DIGITS/4);
}

// TODO tests
void floatn_sqrt_into(floatn f, floatn *r) {
	assert( f.sgn == POS );

	// we need some scratch space
	natural tmpn = {}; 
	floatn tmp = {POS, 0, &tmpn};
	
	// we need some scratch space
	natural tmptwo = {}; natural_from_wide_into(2, &tmptwo);
	floatn two = {POS, 0, &tmptwo};

	// intial guess, x0 = f with exponent in half
	natural tmpxn = {}; 
	floatn xn = {POS, f.exp/2, &tmpxn};
	memcpy(xn.man, f.man, sizeof(natural));

	// ie, xn+1 = 1/2(xn + f/xn)
	// TODO number of iterations: try a couple
	// at low precision, then a few at high precision
	// high precision and compare
	floatn prev = floatn_new();
	int precision = 2;
	for (int i=0; i<100; i++) {
		floatn_divide_into_setprecision(f, 
				xn, &tmp, precision);
		floatn_add_into(xn, tmp, &xn);
		floatn_divide_into_setprecision(xn, two, &xn, precision);

		if ( natural_eq(prev.man, xn.man) ) break;
		memcpy(prev.man, xn.man, sizeof(natural));
		prev.exp = xn.exp;
	}


	precision = 8;
	for (int i=0; ; i++) {
		floatn_divide_into_setprecision(f, 
				xn, &tmp, precision);
		floatn_add_into(xn, tmp, &xn);
		floatn_divide_into_setprecision(xn, two, &xn, precision);

		if ( natural_eq(prev.man, xn.man) ) {
			if (precision == 103) {
				break;
			}

			precision *= 2;
			precision = precision > 103 ? 103: precision;
		}

		memcpy(prev.man, xn.man, sizeof(natural));
		prev.exp = xn.exp;
	}

	memcpy(r->man, xn.man, sizeof(natural));
	r->exp = xn.exp;
}

void floatn_sqrt_ip(floatn *f) {
	floatn_sqrt_into(*f, f);
}

// tidy this up
// want arctan(x) = 2arctan( x / 1+sqrt(1+x^2) )
void floatn_half_angle_op_into(floatn f, floatn *r) {
	natural tmpn = {}; natural_from_wide_into(1, &tmpn);
	floatn one = {POS, 0, &tmpn};

	floatn denom = floatn_new();
	floatn_pow_into(f, 2, &denom);

	floatn_add_ip(&denom, one);
	floatn_sqrt_ip(&denom);

	floatn_add_ip(&denom, one);


	floatn_divide_into_setprecision(f, denom, r, MAX_DIGITS/4);
}

void floatn_multiply_into_setprecision(floatn f, floatn g, floatn *r, int prec) {
	int ndigits = (f.man->c < g.man->c ? f.man->c : g.man->c); 
	natural_multiply_into(f.man, g.man, r->man);
	r->exp = f.exp + g.exp;
	r->sgn = f.sgn ^ g.sgn;
	// and we round post-hoc, since we didn't do truncated mult
	floatn_round_ip(r, prec);
}

void floatn_arctan_into(floatn f, floatn *r) {
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
	} else {
		// don't bother
		memcpy(z.man, f.man, sizeof(natural) );
		z.exp = f.exp; z.sgn = f.sgn;
	}

	/*printf("inverted: "); floatn_println(z);*/

	// now, perform the maybe less obvious range 
	// reduction, TODO if close to 1 
	int halvings = 8;
	for (int i=0; i<halvings; i++) {
		floatn_half_angle_op_into(z, &z);
	}

	/*printf("z: %f\n", floatn_to_double(z));*/

	// so, here z is argument. Taylor
	floatn sum = floatn_new();	
	floatn term = floatn_new();	
	floatn denomf = floatn_new();

	floatn z2 = floatn_new();
	floatn_pow_into(z, 2, &z2);

	floatn zpower = floatn_from_wide(1);

	//arctan(z) = SUM( -1^n * z^(2n+1) / 2n+1)
	for(int n=0; n < 400; n++) {
		/*printf("Sum: "); floatn_println(sum);*/

		//set up term
		bool sgn = n%2 == 0 ? POS : NEG; 
		wide denom = 2*n + 1;

		floatn_from_wide_into(denom, &denomf);
		//floatn_pow_into(z, denom, &term); //can make this much faster, if need be
		floatn_multiply_into_setprecision(z, zpower, &term, 105); //here, it is faster
		floatn_multiply_ip_setprecision(&zpower, z2, 105);

		floatn_divide_into_setprecision(term, denomf, &term, 103);

		term.sgn = sgn;

		/*printf("Term: "); floatn_println(term);*/

		if ( (term.exp + term.man->c) < -103) break;
		
		//add it to accumulator
		floatn_add_ip(&sum, term); // dbl check ip works
		floatn_round_ip(&sum, 103);
	}

	memcpy(r->man, sum.man, sizeof(natural));
	r->exp = sum.exp; r->sgn = sum.sgn;

	// correct half-angle reduction
	/*printf("result, pre correction: "); floatn_println(*r);*/
	floatn_multiply_ip_setprecision(r, floatn_from_wide(pow(2,halvings)), MAX_DIGITS/4);
	/*printf("result: "); floatn_println(*r);*/
	
	// inverse range reduction - reciprocal
	if (inverted) {
		r->sgn = NEG;
		floatn_add_into(pii2, *r, r); 
	}
	/*printf("full: "); floatn_println(*r);*/
	
}
