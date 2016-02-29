#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>

#define MAX_DIGITS 128
#define SLIM_MAX INT32_MAX;
#define WIDE_MAX INT64_MAX;
#define BASE 10

//TODO get a .h, split up the codez a little

// define local aliases for int32 and int64
// both for laziness, and in case I need
// to change them
typedef int32_t slim;
typedef int64_t wide;

// natural is our basic numerical type.
// holds digits in a fixed-sized array, keeps the
// index of the highest non-zero digit in natural.c.
// .c is equivalent to numdigs
typedef struct natural {
	slim c; 
	slim digits[MAX_DIGITS];
} natural;

natural *natural_new();
natural *natural_from_wide(wide x);
void natural_from_wide_into(wide x,natural *n);
wide natural_to_wide(natural *n);

void natural_count(natural *n);
void natural_normalize(natural *n);
bool natural_is_valid(natural *n);

void wides_into_slims(slim *sp,wide *wp,wide *wend);
wide slims_to_wide(slim *p,slim *end);
double slims_to_double(slim *p,slim *end);
double natural_to_double(natural *n); 
void wides_normalize(wide *p,wide *end);

#define GRTR 1
#define EQUAL 0
#define LESS -1
int natural_compare(natural *n,natural *m);
bool natural_eq(natural *n,natural *m);
bool natural_lt(natural *n,natural *m);
bool natural_gt(natural *n,natural *m);
void natural_print(natural *n);
void natural_println(natural *n);

typedef struct qandr {
	natural *q;
	natural *r;
} qandr;

qandr natural_divide_rem(natural *n,natural *m);
void natural_divide_into(natural *n, natural *m, natural *r);
void natural_divide_helper(natural *n,natural *m,natural *product,natural *q);
qandr natural_divide_single(natural *n,slim x);

natural *natural_multiply(natural *n,natural *m);
void natural_multiply_ip(natural *n,natural *m);
void natural_multiply_into(natural *n,natural *m,natural *r);

natural *natural_subtract(natural *n,natural *m);
void natural_subtract_ip(natural *n,natural *m);
void natural_subtract_into(natural *n,natural *m,natural *r);

natural *natural_add(natural *n,natural *m);
void natural_add_ip(natural *n,natural *m);
void natural_add_into(natural *n,natural *m,natural *r);

natural *natural_shift(natural *n,int x);
void natural_shift_into(natural *n,int x,natural *r);
void natural_shift_ip(natural *n,int x);

