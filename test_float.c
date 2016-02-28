#include "float.c"

void print_binary(double d) {
	unsigned long long bit = 1;
	for (int i=0; i<63; i++) {
		printf("%d", 
				(bool) (((unsigned long long) d)>>i & bit));
	}
}

int main(int argc, char *argv[]) {
	double d = 1;
	uint32_t x;
	double_split f;

	// this is the official way of cheating
	memcpy(&x, &d, sizeof(d));
	memcpy(&f, &d, sizeof(d));

	/*printf("neg %x\n", (x>>31) !=0);*/
	/*printf("man %x\n", (x & ((1<<23) -1)));*/
	/*printf("exp %x\n", (x>>23) & 0xFF );*/

	printf("neg %d\n", f.neg);
	printf("exp %d\n", f.exp);
	printf("man %d\n", f.man);

	printf("%f\n", d);

	return 0;
}
