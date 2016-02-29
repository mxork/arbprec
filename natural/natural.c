#include "natural.h"

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

// attempt to pack a natural into a wide;
// overflow will be signaled by crashing.
wide natural_to_wide(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	return slims_to_wide(p, end);
}

double natural_to_double(natural *n) {
	slim *p = n->digits;
	slim *end = n->digits + n->c;

	return slims_to_double(p, end);
}

// natural_print pretty prints a natural!
void natural_print(natural *n) {
	for(int i=n->c-1; i>=0; i--) {
		printf("%d ", n->digits[i]);
	}
}

void natural_println(natural *n) {
	for(int i=n->c-1; i>=0; i--) {
		printf("%d ", n->digits[i]);
	}
	printf("\n");
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


