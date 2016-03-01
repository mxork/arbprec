#include "floatn.h"

// TODO break up into subfiles

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
	slim *end = n->digits + n->c;

	while(p < end && *p == 0) p++;

	return (int) (p - n->digits); 
}

bool natural_is_zero(natural *n) {
	return n->c == 0;
}

// correct exp by number of zeroes, return
// amount of shift
int floatn_pop_zeroes_ip(floatn *f) {
	int nzeroes = natural_zeroes_on_right(f->man);
	if (nzeroes == MAX_DIGITS) {
		f->exp = 0;
		return MAX_DIGITS;
	}
	/*printf("in pop zero call %d\n", nzeroes);*/
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

floatn floatn_from_wide(wide x) {
	natural *m = natural_from_wide(x);
	floatn f = {POS, 0, m};
	floatn_pop_zeroes_ip(&f);
	return f;
}

void floatn_from_wide_into(wide x, floatn *f) {
	natural_from_wide_into(x, f->man);
	f->sgn = POS; f->exp = 0;
	floatn_pop_zeroes_ip(f);
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

double floatn_to_double(floatn f) {
	double x = natural_to_double(f.man);
	x *= pow(BASE, f.exp);
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

void floatn_add_ip(floatn *f, floatn g) {
	floatn_add_into(*f, g, f);
}

floatn floatn_add(floatn f, floatn g) {
	floatn r = floatn_new();
	floatn_add_into(f, g, &r);
	return r;
}

// in the case of the mantissa of a float,
// we can guarantee that n->c agrees with number
// of digits
void floatn_round_ip(floatn *f, int s) {
	assert(s > 0);
	int c = f->man->c;
	if (c <= s ) return; // already low precision; round unecessary


	// we could do this more directly, but it works, so whatevs.
	// TODO
	int diff = c-s;

	assert( diff > 0);
	assert( diff < f->man->c);

	// least-sig dig
	slim *lsdp = f->man->digits + diff;

	// round up?
	if (*(lsdp-1) >= (BASE/2)) {
		(*lsdp)++;	
		slim *p = lsdp;
		while (*p == BASE) {
			*p = 0;
			*(p+1) += 1;
			p++;
			if (p == f->man->digits + f->man->c) f->man->c++; // incase we increment above existing range
		}
	} 
	
	slim *p = f->man->digits;
	while(p < lsdp) {
		*p = 0;		
		p++;
	}

	//memset(f->man->digits, 0, diff); 
	////BUG weird that while loop above works, and memset didn't
	floatn_pop_zeroes_ip(f);
}

void floatn_multiply_into(floatn f, floatn g, floatn *r) {
	int ndigits = (f.man->c < g.man->c ? f.man->c : g.man->c); 
	natural_multiply_into(f.man, g.man, r->man);
	r->exp = f.exp + g.exp;
	r->sgn = f.sgn ^ g.sgn;
	// and we round post-hoc, since we didn't do truncated mult
	floatn_round_ip(r, ndigits);
}

void floatn_multiply_ip(floatn *f, floatn g) {
	int ndigits = f->man->c < g.man->c ? f->man->c : g.man->c;
	natural_multiply_ip(f->man, g.man);
	f->exp += g.exp;
	f->sgn ^= g.sgn;
	floatn_round_ip(f, ndigits);
}

void floatn_multiply_ip_setprecision(floatn *f, floatn g, int prec) {
	natural_multiply_ip(f->man, g.man);
	f->exp += g.exp;
	f->sgn ^= g.sgn;
	floatn_round_ip(f, prec);
}


// screw the remainder
void floatn_divide_into(floatn f, floatn g, floatn *r) {
	// gonna be modifying, so make allocations
	// actually we don't touch g
	natural tmpf = {};//, tmpg = {};
	natural *fm = &tmpf, *gm = g.man;
	memcpy(fm, f.man, sizeof(natural)); 
	//memcpy(gm, g.man, sizeof(natural));

	// get the two aligned, plus a guard digit. Pretty sure
	// we only need one here, since I assume that we get 
	// | n->c - m->c | digits in the integral quotient, whereas we may,
	// reasonably, pick up | n->c - m->c | +1.
	int ndigits = fm->c > gm->c ? gm->c : fm->c; // output will have ndigits
	int s = ndigits + 1; // 1==guard digits. 

	int shift = s - (fm->c - gm->c); // ie if g is longer, need more shift on f
	natural_shift_ip(fm, shift); // hereon fm->c may lie
	natural_divide_into(fm, gm, r->man); 

	// set exp on r: ie. 178/123 -> 0.abc(d), d is guard, s = 4
	// rounding will correct
	r->exp = f.exp - g.exp - (shift);

	// clean up r
	floatn_round_ip(r, ndigits);

	// and, finally, the sign
	r->sgn = f.sgn ^ g.sgn;
};

void floatn_pow_into(floatn f, int power, floatn *r) {
	assert(power >= 0);
	r->exp = 0;
	r->sgn = POS;
	natural_from_wide_into(1, r->man);

	natural nb = {};
	floatn tmp = {f.sgn, f.exp, &nb};
	memcpy(tmp.man, f.man, sizeof(natural));

	// TODO, is there a decent bound for number of digits to keep?
	while (power > 0) {
		if (power % 2) floatn_multiply_ip_setprecision(r, tmp, MAX_DIGITS/4);
		floatn_multiply_ip_setprecision(&tmp, tmp, MAX_DIGITS/4);
		power /= 2;
		/*printf("rc: %d\n", r->man->c);*/
		/*printf("tc: %d\n", tmp.man->c);*/
	}

	floatn_round_ip(r, MAX_DIGITS/4); // TODO no rounding? okay
}
