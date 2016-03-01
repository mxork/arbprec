#include "float/floatn.h"
#include <time.h>
#include <float.h>

#define NTESTS 5

void test_add() {
	natural nb = {}, mb ={}, rb={};
	natural *n = &nb, *m = &mb, *rn=&rb;

	for (int i=0; i<NTESTS; i++) {
		wide x = rand();
		wide y = rand();

		natural_from_wide_into(x, n);
		natural_from_wide_into(y, m);

		floatn f = floatn_from_natural(n);
		floatn g = floatn_from_natural(m);
		g.sgn = NEG;

		/*printf("Floats\n");*/
		/*floatn_println(f);*/
		/*floatn_println(g);*/

		floatn r = floatn_from_natural(rn);
		/*floatn_println(r);*/

		floatn_add_into(f,g,&r);

		if (x-y != floatn_to_wide(r)) {
			printf("-----------------\n");
			printf("Naturals\n");
			natural_println(n);
			natural_println(m);
			printf("Floats\n");
			floatn_println(f);
			floatn_println(g);
			floatn_println(r);
			printf("Result: \n");
			floatn_println(r);
			printf("result: %ld\n", floatn_to_wide(r));
			printf("expect: %ld\n", x-y);
			printf("FAIL\n");
		} else {
			//printf("Add test passed %d\n", i);
		}
		free(f.man);
		free(g.man);
		free(r.man);
	}
}

void test_divide() {
	natural nb = {}, mb ={}, rb={};
	natural *n = &nb, *m = &mb, *rn=&rb;

	// TODO kinda weak, only one sign checked
	for (int i=0; i<NTESTS; i++) {
		wide x = rand();
		wide y = rand();

		double xd = x;
		double yd = y;

		natural_from_wide_into(x, n);
		natural_from_wide_into(y, m);

		floatn f = floatn_from_natural(n);
		floatn g = floatn_from_natural(m);
		/*printf("Floats preop\n");*/
		/*floatn_println(f);*/
		/*floatn_println(g);*/
		g.sgn = NEG;

		/*printf("Floats postroun\n");*/
		/*floatn_println(f);*/
		/*floatn_println(g);*/

		floatn r = floatn_from_natural(rn);
		/*floatn_println(r);*/

		floatn_divide_into(f,g,&r);

		double expect = xd/(-yd);

		// if rel error > 0.1, shit flip
		if  ( (expect - floatn_to_double(r)) / expect > 0.01 ) {
			printf("-----------------\n");
			printf("Naturals\n");
			natural_println(n);
			natural_println(m);
			printf("Floats\n");
			floatn_println(f);
			floatn_println(g);
			floatn_println(r);
			printf("Result: \n");
			floatn_println(r);
			printf("result: %f\n", floatn_to_double(r));
			printf("expect: %f\n", expect);
			printf("FAIL\n");
		} else {
			//printf("DIV test passed %d\n", i);
		}
		free(f.man);
		free(g.man);
		free(r.man);
	}
}

void test_round() {
	natural nb = {}, mb ={}, rb={};
	natural *n = &nb, *m = &mb, *rn=&rb;

	// TODO kinda weak, only one sign checked
	for (int i=0; i<1; i++) {
		wide x = 459997308; //100000*((double) rand())/((double) RAND_MAX);

		natural_from_wide_into(x, n);

		floatn f = floatn_from_natural(n);
		f.sgn = NEG;

		int s = 4; // number of digits to round to

		// this may break in other bases
		floatn_round_ip(&f, s);

		if (true) {

			printf("wide: %ld\n", x);
			printf("Float, rounded\n");
			floatn_println(f);
			printf("floatn: %f\n", floatn_to_double(f));
		}


		printf("\n");

		/*floatn_println(r);*/
		free(f.man);
	}

}

void test_multiply() {
	natural nb = {}, mb ={}, rb={};
	natural *n = &nb, *m = &mb, *rn=&rb;

	// TODO kinda weak, only one sign checked
	for (int i=0; i<NTESTS; i++) {
		wide x = rand();
		wide y = rand();

		natural_from_wide_into(x, n);
		natural_from_wide_into(y, m);

		floatn f = floatn_from_natural(n);
		floatn g = floatn_from_natural(m);
		g.sgn = NEG;

		/*printf("Floats\n");*/
		/*floatn_println(f);*/
		/*floatn_println(g);*/

		floatn r = floatn_from_natural(rn);
		/*floatn_println(r);*/

		floatn_multiply_into(f,g,&r);

		if  ( ((double) x*(-y)-floatn_to_wide(r))/(x*(-y)) > 0.01 ) {
			printf("-----------------\n");
			printf("Naturals\n");
			natural_println(n);
			natural_println(m);
			printf("Floats\n");
			floatn_println(f);
			floatn_println(g);
			floatn_println(r);
			printf("Result: \n");
			floatn_println(r);
			printf("result: %ld\n", floatn_to_wide(r));
			printf("expect: %ld\n", x*(-y));
			printf("FAIL\n");
		} else {
			//printf("Mult test passed %d\n", i);
		}
		free(f.man);
		free(g.man);
		free(r.man);
	}
}

void test_pow() {
	natural nb = {}, mb ={}, rb={};
	natural *n = &nb, *m = &mb, *rn=&rb;

	// TODO kinda weak, only one sign checked
	for (int i=0; i<NTESTS; i++) {
		wide x = rand();
		int y = 13;

		natural_from_wide_into(x, n);

		floatn f = floatn_from_natural(n);

		/*printf("Floats\n");*/
		floatn_println(f);

		printf("f->c: %d\n", f.man->c);

		floatn r = floatn_from_natural(rn);

		floatn_pow_into(f,y,&r);

		floatn_println(r);
		printf("r->c: %d\n", f.man->c);
		printf("result: %f\n", floatn_to_double(r));
		printf("expect: %f\n", pow(-x,y));

		if ((pow(-x,y) - floatn_to_double(r))/pow(x,y) > 0.1) {
			printf("-----------------\n");
			printf("Naturals\n");
			natural_println(n);
			printf("Floats\n");
			floatn_println(f);
			printf("Result: \n");
			floatn_println(r);
			printf("result: %f\n", floatn_to_double(r));
			printf("expect: %f\n", pow(-x,y));
			printf("FAIL\n");
		} else {
			//printf("Pow test passed %d\n", i);
		}
		free(f.man);
		free(r.man);
	}

}

void main(int argc, char *argv[]) {
	srand(time(NULL));
	/*test_add();*/
	/*test_multiply();*/
	/*test_divide();*/
	test_pow();
	/*test_round();*/
}
