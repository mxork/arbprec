#include <time.h>
#include "arctan/arctan.c"

#define NTESTS 1000

void test_invert() {
	wide x = rand();
	printf("%ld\n", x);

	floatn tmp = floatn_new();
	floatn swp = {};
	floatn f = floatn_from_natural(natural_from_wide(x));
	floatn_pow_into(f, 13, &tmp);
	swp = tmp; tmp = f; f = swp;
	

	floatn r = floatn_new();

	floatn_invert_into(f, &r);

	// so r == 1/ x^13
	floatn_println(r);
}

void main(int argc, char *argv[]) {
	srand(time(NULL));
	test_invert();
}
