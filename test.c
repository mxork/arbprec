#include <time.h>
#include "natural.c"

// checkout gmp

typedef struct test_result {
	bool failed;
	wide expected, got;
	char *opname;
} test_result;

typedef test_result (*testfp)(wide,wide,natural *, natural *);


// rand guaranteed 16 bits
wide rand_wide() {
	int16_t mask = ~0; //~ == bitwise NOT

	// stick some rands together,
	// being careful with the signage
	// on the msb
	// keep it short ot avoid overflow
	return 
		//(((wide) rand() & (mask>>1)) << 47) |
		(((wide) rand() & mask) << 32) |
		(((wide) rand() & mask) << 16) |
		(((wide) rand() & mask));
}

bool test_spam(testfp test, int n) {
	for (int i=0; i<n; i++) {
		wide x = rand_wide();
		wide y = rand_wide();
		natural *n = natural_from_wide(x);
		natural *m = natural_from_wide(y);

		test_result result = test(x,y,n,m);

		const char *fmt = 
			"%s \t%d %d\n"
			"\t\t= %d\n"
			"but";

		if (result.failed) {
			printf(fmt, result.opname, x, y, result.expected);
			natural_print(n);
			printf("\n");
			natural_print(m);
			printf("\n");
		}
	}
}

test_result test_add(wide x, wide y, natural *n, natural *m) {
	wide answer = natural_to_wide(natural_add(n, m));
	test_result result = {
		.failed = !(answer == x+y),	
		.expected = x+y,	
		.got = answer,
		.opname = "add",	
	};

	return result;
}

test_result test_subtract(wide x, wide y, natural *n, natural *m) {
	wide answer = natural_to_wide(natural_subtract(n, m));
	test_result result = {
		.failed = !(answer == x-y),	
		.expected = x-y,	
		.got = answer,
		.opname = "subtract",	
	};

	return result;
}

bool test_compare(wide x, wide y, natural *n, natural *m) {
	int answer = natural_compare(n, m);
	switch (answer) {
	case GRTR:
		return (x > y);
		break;
	case LESS:
		return (x < y);
		break;
	case EQUAL:
		return (x == y);
		break;
	}
}

int main(int argc, char *argv[]) {
	srand((unsigned int)time(NULL));

	int ntest = 20;
	test_spam(&test_add, ntest);
	test_spam(&test_subtract, ntest);
}
