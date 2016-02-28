#include "natural.h"

qandr natural_divide_single(natural *n, slim x) {
	assert(x < BASE && x > 0);
	natural *q = natural_new();
	natural *r = natural_new();
	qandr qr = {q, r};

	slim *np = n->digits + n->c -1;
	slim *qp = q->digits + n->c -1;
	wide carry = 0;

	while (np >= n->digits) {
		wide partial_q = carry*BASE + *np;
		*qp = partial_q / x;
		carry = partial_q % x; 

		np--; qp--;
	}

	natural_count(q);
	natural_from_wide_into(carry, r);
	return qr;
}

// n == remainder, m == divisor, product == recycled buffer, q == quotient
void natural_divide_helper(natural *n, natural *m, natural * product, natural *q) {
	// special cases: each makes 2 fresh allocations that will
	// be passed all the way up the recursion chain
	if (natural_lt(n,m)) {
		// and done
		return;
	}

	if (natural_eq(n,m)) {
		natural temp = {};
		natural_from_wide_into(1, &temp);
		natural_add_ip(q, &temp);
		return;
	}

	assert(m->c != 0); 
	if (m->c == 1) {
		qandr temp = natural_divide_single(n,m->digits[0]);
		natural_add_ip(q, temp.q); n = temp.r;
		free(temp.q);
		return;
	}

	// moving to the main case
	assert(n->c >= 2 && m->c >=2 && natural_gt(n,m));

	// do prefixes
	wide nprefix = slims_to_wide(n->digits + n->c - 2, n->digits + n->c);
	wide mprefix = slims_to_wide(m->digits + m->c - 2, m->digits + m->c);
	int shift = n->c - m->c;
	
	 /*grab another digit if necessary, move quotient digit respectively*/
	if (nprefix <= mprefix) {
		nprefix = slims_to_wide(n->digits + n->c - 3, n->digits + n->c);
		shift--;
	}

	// test quotient digit
	wide qt = nprefix / mprefix;

	// correct off by one
	natural qtnbacking= {}; natural *qtn = &qtnbacking;
	natural_from_wide_into(qt, qtn); 
	natural_shift_ip(qtn, shift);
	natural_multiply_into(qtn, m, product);

	if ( natural_gt(product, n) ) {
		qt -= 1;
		natural_from_wide_into(qt, qtn);
		natural_shift_ip(qtn, shift); 
		natural_multiply_into(m, qtn, product);
	}

	natural *r = n; // just an alias - no copy

	// form answers
	natural_subtract_ip(r, product);
	natural_add_ip(q, qtn);

	// recur -> I hear gcc has tail-call opt
	natural_divide_helper(r, m, product, q);
}

// allocates and returns 2 naturals: q and r.
qandr natural_divide_rem(natural *n, natural *m) {
	natural *quotient = natural_new();
	natural *remainder = natural_new();
	memcpy(remainder, n, sizeof(natural));

	natural *product = natural_new();

	natural_divide_helper(remainder,m,product,quotient);

	free(product);

	qandr qr = {quotient, remainder};
	return qr;
}

// attempt to convert a slice of slims into
// a single wide
wide slims_to_wide(slim *p, slim *end) {
	wide shift = 1;
	wide sum = 0;

	while (p < end) {
		sum += (*p) * shift;
		assert(sum >= 0); // overflow?
		shift *= BASE;
		p++;
	}

	return sum;
}
