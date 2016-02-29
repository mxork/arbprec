#include "natural.h"

// allows negative shift, in place.
void natural_shift_ip(natural *n, int x) {
	if (x == 0) return;
	memset(n->digits + n->c, 0, MAX_DIGITS - n->c); //simple solutions

	if (x < 0) {
		slim *dst = n->digits;
		slim *src = n->digits - x;

		while (dst < n->digits + n->c) {
			*dst = *src;
			*src = 0; // FIXME shouldn't be necessary, but 
			// not sure how to handle non-zero bits outside range
			src++; dst++;
		}

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
	}

	n->c += x;
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
