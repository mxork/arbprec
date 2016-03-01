#include "natural.h"

// grade school
void natural_multiply_into(natural *n, natural *m, natural *r) {
	assert( (n->c + m->c) < MAX_DIGITS);
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
		assert(*wp < BASE);
		*sp = (slim) *wp;	
		sp++; wp++; 
	}
}
