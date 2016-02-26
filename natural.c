#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define MAX_DIGITS 1024
#define BASE 10

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

wide natural_to_wide(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	wide shift = 1;
	wide sum = 0;

	while (p < end) {
		sum += (*p) * shift;
		assert(sum > 0);
		shift *= BASE;
		p++;
	}

	return sum;
}

// natural_print pretty prints a natural.
void natural_print(natural *n) {
	for(int i=0; i<n->c; i++) {
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
	slim carry = 0;
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	n->c =0; // we'll recalculate this

	while (carry > 0 || p < end) {
		*p += carry;
		carry = *p / BASE;
		*p %= carry;

		p++; 
		n->c++; //keep no of digits current
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
		np++; mp++; rp++; 
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

	if ( natural_lt(n, m) ) return r; // no negatives

	slim *np= n->digits;
	slim *mp= m->digits;
	slim *rp= r->digits;

	slim *end= n->digits + n->c;

	while (np < end) {
		// grab one from higher order term if necessary
		if (*np < *mp) {
			*(mp+1) += 1;
			*mp += BASE;
		}

		*rp = *np - *mp;

		np++; mp++; rp++; r->c++;
	}

	return r;
}

void natural_free(natural *n) {
	free(n->digits);
	free(n);
}

natural *natural_multiply(natural *n, natural *m) {
	natural *r = natural_new();

}

