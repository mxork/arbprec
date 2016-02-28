#include "natural.h"

void natural_add_into(natural *n, natural *m, natural *r) {
	slim *np = n->digits;
	slim *mp = m->digits;
	slim *rp = r->digits;

	slim *nend = n->digits + n->c;
	slim *mend = m->digits + m->c;

	// sum over shared range
	while (np < nend && mp < mend) {
		*rp = *np + *mp;	
		np++; mp++; rp++;
	}

	// only one of these two will do
	// anything, depending on lengths
	while (np < nend) {
		*rp = *np;
		np++; rp++;
	}

	while (mp < mend) {
		*rp = *mp;
		mp++; rp++;
	}

	natural_normalize(r);
}

// natural_add
natural *natural_add(natural *n, natural *m) {
	natural *r = natural_new();
	natural_add_into(n,m,r);
	return r;
}

void natural_add_ip(natural *n, natural *m) {
	natural_add_into(n, m, n);
}


// natural_subtract in place
void natural_subtract_into(natural *n, natural *m, natural *r) {
	assert( !natural_lt(n,m) );

	slim *np= n->digits;
	slim *mp= m->digits;
	slim *rp= r->digits;

	slim *nend= n->digits + n->c;
	slim *mend= m->digits + m->c;

	int carry = 0;
	while (mp < mend) {
		int sub_digit = *mp + carry;
		carry = 0;

		// grab one from higher order term if necessary
		if (*np < sub_digit) {
			carry++;
		}

		*rp = *np + carry*BASE - sub_digit;
		np++; mp++; *rp++;
	}

	// handle those digits of n hanging above zeroes
	// we could just memcpy after the first and be happy, but whatever
	while (np < nend) {
		int sub_digit = carry; //ie, *mp == 0 beyond m->c
		carry = 0;

		if (*np < sub_digit) {
			carry++;
		}
		
		*rp = *np + carry*BASE - sub_digit;
		np++; rp++;
	}

	// necessary for count
	memset(rp, 0, (size_t) ((r->digits + MAX_DIGITS) - rp));

	natural_count(r);
}

void natural_subtract_ip(natural *n, natural *m) {
	natural_subtract_into(n, m, n);
}

natural *natural_subtract(natural *n, natural *m) {
	natural *r = natural_new();
	natural_subtract_into(n,m,r);
	return r;
}
