#ifndef __FIXED_SIN_COS__
#define __FIXED_SIN_COS__

// Original is 'https://github.com/dsnet/tri-approx'

#ifndef M_PI
#define	M_PI 	(3.141593)
#endif
#ifndef assert
#define assert(__e) ((__e) ? (void)0 : Printf (__FILE__, __LINE__, #__e))
#endif
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(int)(sizeof(x) / sizeof((x)[0]))
#endif

int64_t sine(int64_t value);
int64_t cosine(int64_t value);
double fixed_to_float(int64_t value, int scale);
int64_t float_to_fixed(double value, int scale);

#endif
