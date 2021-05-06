#ifndef __FP_SINCOS__
#define __FP_SINCOS__

// https://www.nullhardware.com/blog/fixed-point-sine-and-cosine-for-embedded-systems/

#include <stdint.h>

/*
 * sine degree : 0 ~ 90 ~ 180 ~ 270 ~ 360 
 * sine value  : 0 ~  1 ~  0  ~ -1  ~  0
 *
 * fpsine 	   : 0 ~ 8192 ~ 16384 ~ 24576 ~ 32768 
 * fpsine value: 0 ~ 4096 ~   0   ~ -4096 ~  0
 */

int16_t fpsin(int16_t i);
/* Cos(x) = sin(x + pi/2) */
#define fpcos(i) fpsin((int16_t)(((uint16_t)(i)) + 8192U))

#define __round(x) 		((x) >= 0 ? (long)((x) + 0.5) : (long)((x) - 0.5))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(int)(sizeof(x) / sizeof((x)[0]))
#endif

#endif
