#include "natural.c"

int main(int argc, char *argv[]) {
	int ntests;
	scanf("%d", &ntests);
	assert(ntests);


	for (int i=0; i < ntests; i++) {
		int testcase;
		scanf("%d", &testcase);
		natural *n = natural_int(testcase);

		natural_print(n);
		printf("\n");
	}
}
