#include "float.c"

int main(int argc, char *argv[]) {
	double d = -10293.1283123;
	floatn f = floatn_from_double(d);
	printf("%f\n", d);
	floatn_println(f);

	return 0;
}
