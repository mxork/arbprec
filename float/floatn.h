#include "../natural/natural.h"

#define POS 0
#define NEG 1

typedef struct floatn {
	bool sgn; // POS and NEG
	int exp; //exponent
	natural *man; //mantissa
} floatn;

floatn floatn_new();
floatn floatn_from_natural(natural *n);
int natural_zeroes_on_right(natural *n);
void floatn_println(floatn f);
void floatn_print(floatn f);
