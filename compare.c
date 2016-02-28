#include "natural.h"

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

