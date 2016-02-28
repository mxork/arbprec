#include "float/floatn.c"
#include <time.h>

#define NTESTS 100000

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
			printf("Add test passed %d\n", i);
		}
		free(f.man);
		free(g.man);
		free(r.man);
	}
}

void main(int argc, char *argv[]) {
	srand(time(NULL));
	/*natural *n = natural_from_wide( (wide) rand() );*/
	/*natural *m = natural_from_wide( (wide) rand() );*/
	test_add();

}
