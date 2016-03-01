#include <time.h>
#include "arctan/arctan.c"
#include <math.h>

#define NTESTS 5

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

// passing, but slow
void test_sqrt() {
	for (int i=0; i< NTESTS; i++) {
		wide x = rand();
		floatn f = floatn_from_wide(x);
		//f.exp = -30;
		floatn r = floatn_new();

		floatn_sqrt_into(f, &r);
		// so r == 1/ x^13
		//


		/*if (rd != sqrt(x) ) {*/
			/*printf("-------\n");*/
			/*printf("FAIL\n");*/
			/*floatn_println(f);*/
			/*floatn_println(r);*/
			/*printf("%f\n", (double) sqrt(x));*/
			/*printf("%f\n", rd);*/
			/*printf("-------\n");*/
		/*}*/
	}
}

void test_arctan_basic() {
	wide x = 9999999;
	floatn f = floatn_new();
	floatn r = floatn_new();
	floatn_from_wide_into(x, &f);
	f.exp = -7;

	floatn_println(f);
	floatn_arctan_into(f, &r);
	floatn_println(r);
}

void test_arctan() {
	floatn f = floatn_new();
	floatn r = floatn_new();

	for (int i=0; i<NTESTS; i++) {
		double xd = ((double) rand()) / ((double) RAND_MAX);
		wide x = (xd * pow(10, 6));

		floatn_from_wide_into(x, &f);
		f.exp = -5;
		floatn_arctan_into(f, &r);

		printf("---------\n");
		floatn_println(r);
		printf("%f\n", (double) atan(xd * 10 ));
		printf("---------\n");

	}
}

void main(int argc, char *argv[]) {
	srand(time(NULL));
	/*test_invert();*/
	/*test_sqrt();*/
	test_arctan();
	/*test_arctan_basic();*/
}
