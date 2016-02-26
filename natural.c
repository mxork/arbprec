#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>

#define MAX_DIGITS 1024
#define SLIM_MAX INT32_MAX;
#define WIDE_MAX INT64_MAX;
#define BASE 10

//TODO get a .h, split up the codez a little

// define local aliases for int32 and int64
// both for laziness, and in case I need
// to change them
typedef int32_t slim;
typedef int64_t wide;

// in c, long is >= 32b, long long >= 64

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
	natural *n = malloc(sizeof(natural));
	assert(n);
	for (int i=0; i < MAX_DIGITS; i++) {
		(n->digits)[i] = 0;
	}

	return n;
}

// *natural_from_int converts a wide  x to a new
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
		//assert(sum < 0); // overflow?
		shift *= BASE;
		p++;
	}

	return sum;
}

// attempt to pack a natural into a wide
wide natural_to_wide(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	return slims_to_wide(p, end);
}

// natural_print pretty prints a natural.
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

// normalize at least up to end
void wide_normalize(wide *p, wide *end) {
	wide carry = 0;

	// hello segfault
	while (carry > 0 || p < end) {
		*p += carry;
		carry = *p / BASE;
		*p %= BASE;

		p++; 
	}
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
	}

	return true;
}

// natural_add
natural *natural_add(natural *n, natural *m) {
	natural *r = natural_new();

	slim *np = n->digits;
	slim *mp = m->digits;
	slim *rp = r->digits;
	slim *end = n->digits + MAX_DIGITS;

	while (np < end) {
		*rp = *np + *mp;	
		np++; mp++; rp++; r->c++; 
	}

	natural_normalize(r);
	return r;
}

// natural_subtract
natural *natural_subtract(natural *n, natural *m) {
	natural *b = malloc(sizeof(natural)); // we're making some destructive changes
	memcpy(b, m, sizeof(natural));
	m = b;

	natural *r = natural_new();

	assert( !natural_lt(n,m) );

	slim *np= n->digits;
	slim *mp= m->digits;
	slim *rp= r->digits;

	slim *end= n->digits + n->c;

	while (np < end) {
		// grab one from higher order term if necessary
		if (*np < *mp) {
			(*(mp+1))++;
			*np += BASE;
		}

		*rp = *np - *mp;

		np++; mp++; rp++; r->c++;
	}

	return r;
}

// grade school
// reuse old naturals for result
natural *natural_multiply_recycle(natural *n, natural *m, natural *r) {
	wide buffer[MAX_DIGITS] = {0};

	for (int i = 0; i < n->c; i++) {
		for (int j = 0; j < m->c; j++) { 
			buffer[i+j] += n->digits[i]*m->digits[j];
			wide_normalize(buffer + i + j, NULL);
		}
	}

	wides_into_slims(r->digits, buffer, buffer + MAX_DIGITS);
	natural_count(r); // for the count

	return r;
}

natural *natural_multiply(natural *n, natural *m) {
	natural *r = natural_new();
	return natural_multiply_recycle(n,m,r);
}


natural *natural_scalar_multiply(natural *n, wide x) {
	wide *buffer[MAX_DIGITS] = {0};
}

typedef struct qandr {
	natural *q;
	natural *r;
} qandr;

qandr natural_divide_rem(natural *n, natural *m) {
	// again, doing some destructive stuff
	wide buffer[MAX_DIGITS] = {0};
	natural *r = natural_new();
	memcpy(r, n, sizeof(natural));

	while (!natural_lt(r, m)) {
		// TODO handle special cases
		
		// main meaty stuff. consult own java for inspiration
		wide rprefix = slims_to_wide(r->digits + r->c - 2, r->digits + r->c);
		wide mprefix = slims_to_wide(m->digits + m->c - 2, m->digits + m->c);
		int shift = r->c - m->c;

		// grab another digit if necessary, move quotient digit respectively
		if (rprefix < mprefix) {
			rprefix = slims_to_wide(r->digits + r->c - 3, r->digits + r->c);
			shift--;
		}

		// test quotient digit
		wide qt = rprefix / mprefix;

		// correct off by one
		natural qtn = { .c = 1 };
		assert(qt < INT32_MAX);
		qtn.digits[0] = (slim) qt;

		natural *product = natural_multiply(m, &qtn);
		if ( natural_gt(product, r) ) {
			qt--;
			assert(qt < INT32_MAX);
			qtn.digits[0] = (slim) qt;

			free(product);
			product = natural_multiply(m, &qtn);
		}

		// move ahead
		buffer[shift] = qt;
		wide_normalize(buffer+shift, NULL);

		free(r);
		r = natural_subtract(r, product);
	}

	// clean up

	// convert intermediate buffer to natural
	natural *q = natural_new();
	wides_into_slims(q->digits, buffer, buffer+MAX_DIGITS);
	natural_normalize(q); // for count

	qandr ret = { .q = q, .r = r};
	return ret;
}
