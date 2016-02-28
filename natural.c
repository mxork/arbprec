#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

// thoughts on maxdig?
#define MAX_DIGITS 128
#define SLIM_MAX INT32_MAX;
#define WIDE_MAX INT64_MAX;
#define BASE 10

//TODO get a .h, split up the codez a little

// define local aliases for int32 and int64
// both for laziness, and in case I need
// to change them
typedef int32_t slim;
typedef int64_t wide;

// natural is our basic numerical type.
// holds digits in a fixed-sized array, keeps the
// index of the highest non-zero digit in natural.c.
// .c is equivalent to numdigs
typedef struct natural {
	slim c; 
	slim digits[MAX_DIGITS];
} natural;

// *natural_new returns a zero'd natural of count zero
natural *natural_new() {
	natural *n = calloc(1, sizeof(natural));
	assert(n);

	return n;
}

// *natural_from_wide converts a wide  x to a new
// natural
natural *natural_from_wide(wide x) {
	natural *n = natural_new();
	slim *p = n->digits;

	while (x > 0) {
		*p = x % BASE;
		x /= BASE;
		p++; n->c++;
	}

	return n;
}

// as above, but puts the result into passed natural*
void natural_from_wide_into(wide x, natural* n) {
	n->c = 0;
	slim *p = n->digits;

	while (x > 0) {
		*p = x % BASE;
		x /= BASE;
		p++; n->c++;
	}
}

void natural_count(natural *n) {
	slim *p = n->digits + MAX_DIGITS;
	n->c = MAX_DIGITS;
	do {
		n->c--;	p--;
	} while (*p == 0);

	n->c++;
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

// attempt to pack a natural into a wide;
// overflow will be signaled by crashing.
wide natural_to_wide(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	return slims_to_wide(p, end);
}

// natural_print pretty prints a natural!
void natural_print(natural *n) {
	for(int i=n->c-1; i>=0; i--) {
		printf("%d ", n->digits[i]);
	}
}

#define GRTR 1
#define EQUAL 0
#define LESS -1
// natural_compare is self-explanatory: returns a
// trichotomy constant.
int natural_compare(natural *n, natural *m) {
	if (n->c > m->c) return GRTR;
	if (n->c < m->c) return LESS;

	assert(n->c == m->c);

	for (int i= n->c-1; i>=0; i--) {
		if ( n->digits[i] > m->digits[i] ) return GRTR;
		else if ( n->digits[i] < m->digits[i] ) return LESS;
	}

	return EQUAL;
}

// natural greater than
bool natural_gt(natural *n, natural *m) {
	return (natural_compare(n, m) == GRTR);
}


// natural less than
bool natural_lt(natural *n, natural *m) {
	return (natural_compare(n, m) == LESS);
}

// natural equals
bool natural_eq(natural *n, natural *m) {
	return (natural_compare(n, m) == EQUAL);
}

// natural_normalize "flattens" the digits of n so
// that each digit is < BASE. 
void natural_normalize(natural *n) {
	natural_count(n);

	slim carry = 0;
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	n->c =0; // we'll recalculate this

	while (carry > 0 || p < end) {
		*p += carry;
		carry = *p / BASE;
		*p %= BASE;

		p++; 
		n->c++; //keep no of digits current
	}
}

// normalize array of wides, AT LEAST up to end.
void wides_normalize(wide *p, wide *end) {
	wide carry = 0;

	// hello segfault
	do {
		*p += carry;
		carry = *p / BASE;
		*p %= BASE;

		p++; 
	} while (carry > 0 || p < end);
}

// assume normalize
void wides_into_slims(slim *sp, wide *wp, wide *wend) {
	while (wp < wend) {
		*sp = (slim) *wp;	
		sp++; wp++; 
	}
}

// natural_is_valid checks if digits are actually < BASE. 
// may throw in other checks as inclined.
bool natural_is_valid(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + MAX_DIGITS;

	while (p < end) {
		if (*p >= BASE) return false;
		if (*p < 0) return false;
	}

	return true;
}

// allows negative shift, in place.
void natural_shift_ip(natural *n, int x) {
	if (x == 0) return;
	if (x < 0) {
		slim *dst = n->digits;
		slim *src = n->digits - x;

		while (dst < n->digits + n->c) {
			*dst = *src;
			*src = 0; // FIXME shouldn't be necessary, but 
			// not sure how to handle non-zero bits outside range
			src++; dst++;
		}
		n->c += x;

	} else if (x > 0) {
		// reverse direction for positive
		slim *dst = n->digits + n->c + x;
		slim *src = n->digits + n->c;
		assert(dst < n->digits + MAX_DIGITS);

		while (src >= n->digits && dst >= n->digits) {
			*dst = *src;
			*src = 0; 
			src--; dst--;
		}
		n->c += x;
	}
}

void natural_shift_into(natural *n, int x, natural *r) {
	r->c = n->c + x;

	slim *dst = r->digits + r->c;
	slim *src = n->digits + n->c;

	assert(dst < r->digits + MAX_DIGITS);

	while (src >= n->digits && dst >= n->digits) {
		*dst = *src;
		src++; dst++;
	}
}

natural *natural_shift(natural *n, int x) {
	natural *r = natural_new();
	natural_shift_into(n, x, r);
	return r;
}

void natural_add_into(natural *n, natural *m, natural *r) {
	slim *np = n->digits;
	slim *mp = m->digits;
	slim *rp = r->digits;
	slim *end = n->digits + MAX_DIGITS;

	while (np < end) {
		*rp = *np + *mp;	
		np++; mp++; rp++;
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

	slim *end= n->digits + n->c;

	int carry = 0;
	while (np < end) {
		int sub_digit = *mp + carry;
		carry = 0;

		// grab one from higher order term if necessary
		if (*np < sub_digit) {
			carry++;
			*np += BASE;
		}

		*rp = *np - sub_digit;
		np++; mp++; *rp++;
	}

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

// grade school
// reuse old naturals for result
void natural_multiply_into(natural *n, natural *m, natural *r) {
	wide buffer[MAX_DIGITS] = {0};

	for (int i = 0; i < n->c; i++) {
		for (int j = 0; j < m->c; j++) { 
			buffer[i+j] += n->digits[i]*m->digits[j];
			wides_normalize(buffer + i + j, NULL);
		}
	}

	wides_into_slims(r->digits, buffer, buffer + MAX_DIGITS);
	natural_count(r); // for the count
}

void natural_multiply_ip(natural *n, natural *m) {
	natural_multiply_into(n,m,n);
}

natural *natural_multiply(natural *n, natural *m) {
	natural *r = natural_new();
	natural_multiply_into(n,m,r);
	return r;
}

// Division gets messy

// as in quotient and remainder
typedef struct qandr {
	natural *q;
	natural *r;
} qandr;

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
	memset(qtn, 0, sizeof(natural)); // pretty sure nfwi zeroes, FIXME

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
