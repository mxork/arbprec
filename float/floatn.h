#include "../natural/natural.h"
#include <math.h>

#define POS 0
#define NEG 1

typedef struct floatn {
	bool sgn; // POS and NEG
	int exp; //exponent
	natural *man; //mantissa
} floatn;

void floatn_add_into(floatn f,floatn g,floatn *rp);
void floatn_add_ip(floatn *f, floatn g);

void floatn_pow_into(floatn f,int power,floatn *r);
void floatn_divide_into(floatn f,floatn g,floatn *r);

void floatn_multiply_into(floatn f,floatn g,floatn *r);
void floatn_multiply_ip(floatn *f,floatn g);
void floatn_multiply_ip_setprecision(floatn *f,floatn g,int extra);
void floatn_round_ip(floatn *f,int s);

floatn floatn_new();
floatn floatn_devour_natural(natural *n);
floatn floatn_from_natural(natural *n);
floatn floatn_from_wide(wide x);
void floatn_from_wide_into(wide x, floatn *f);
int floatn_pop_zeroes_ip(floatn *f);

wide floatn_to_wide(floatn f);
double floatn_to_double(floatn f);

bool natural_is_zero(natural *n);
int natural_zeroes_on_right(natural *n);

void floatn_println(floatn f);
void floatn_print(floatn f);
