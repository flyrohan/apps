#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "fp_sincos.h" 
#include <time.h>

#define FP_DEGREE_RATIO	(91022)	/* 32768/360 = 91.022 * 1000 */
#define UNUSED 			__attribute__((unused)) 

#define RUN_US(_s) 		do { _s = clock(); } while(0)
#define END_US(_s, _t) 	do { _t = clock() - _s; } while (0)
#define PRINT_FLOAT
#define MSG(m...)		printf(m)
#define SUPPORT_FP_SINCOS_APPROX_TESTING
#define SUPPORT_FP_SINCOS_APPROX_SIN
#define SUPPORT_FP_SINCOS_APPROX_COS
#define SUPPORT_FP_SINCOS_APPROX_DEGREE
#define SUPPORT_FP_SINCOS_APPROX_PERF

#define PERF_COUNT      (100000)


#ifdef SUPPORT_FP_SINCOS_APPROX_TESTING
static int fp_sincos_testing(int argc UNUSED, char * const argv[] UNUSED)
{
	int32_t max = 0, min = 0;

	for (uint16_t i = 0; i <= 32768; i++) {
		int32_t s = lround(4096*sin(2*M_PI * i / 32768));
		int16_t s5d = fpsin((int16_t)i);
		int32_t err = s - s5d;

		if(err > max)
			max = err;
		if(err < min)
			min = err;
		MSG("The value of %d is %d - compare %d, diff : %d\n", i, s5d, s, err);
	}

	MSG("sine err min: %d max: %d\n", min, max);
    return 0;
}
#endif

#ifdef SUPPORT_FP_SINCOS_APPROX_SIN
static int fp_sincos_sine(int argc UNUSED, char * const argv[] UNUSED)
{
	int32_t max = 0, min = 0;

	for (uint16_t i = 0; i <= 360; i++) {
		int32_t r = __round((float)(i * FP_DEGREE_RATIO)/1000);
		int32_t s = lround(4096*sin(2*M_PI * i / 360));
		int16_t s5d = fpsin((int16_t)r);
		int32_t err = s - s5d;

		if(err > max)
			max = err;
		if(err < min)
			min = err;
#ifdef PRINT_FLOAT
		MSG("degree:%3d [%5d] %+f : %+f, err %+f\n",
			i, r, (float)s5d/4096, (float)s/4096, (float)err/4096);
#endif
		MSG("degree:%3d [%5d] %c%d.%06d : %c%d.%06d, err %c%d.%06d\n",
			i, r,
			s5d >= 0 ? '+' : '-',
			abs((int32_t)(((float)s5d/4096)*1000000)/1000000),
			abs((int32_t)(((float)s5d/4096)*1000000)%1000000),
			s >= 0 ? '+' : '-',
			abs((int32_t)(((float)s/4096)*1000000)/1000000), 
			abs((int32_t)(((float)s/4096)*1000000)%1000000), 
			err >= 0 ? '+' : '-',
			abs((int32_t)(((float)err/4096)*1000000)/1000000),
			abs((int32_t)(((float)err/4096)*1000000)%1000000));
	}
#ifdef PRINT_FLOAT
	MSG("sin err min: %f max: %f\n", (float)min/4096, (float)max/4096);
#endif
	MSG("sin err min: %c%d.%06d max: %c%d.%06d\n", 
		min >= 0 ? '+' : '-',
		abs((int32_t)(((float)min/4096)*1000000)/1000000), 
		abs((int32_t)(((float)min/4096)*1000000)%1000000), 
		max >= 0 ? '+' : '-',
		abs((int32_t)(((float)max/4096)*1000000)/1000000), 
		abs((int32_t)(((float)max/4096)*1000000)%1000000));

	return 0;
}
#endif

#ifdef SUPPORT_FP_SINCOS_APPROX_COS
static int fp_sincos_cosine(int argc UNUSED, char * const argv[] UNUSED)
{
	int32_t max = 0, min = 0;

	for (uint16_t i = 0; i <= 360; ++i) {
		int32_t r = __round((float)(i * FP_DEGREE_RATIO)/1000);
		int32_t c = lround(4096*cos(2*M_PI * i / 360));
		int16_t s5d = fpcos(r);
		int32_t err = c - s5d;
		if(err > max)
			max = err;
		if(err < min)
			min = err;
#ifdef PRINT_FLOAT
		MSG("degree:%3d [%5d] %+f : %+f, err %+f\n",
			i, r, (float)s5d/4096, (float)c/4096, (float)err/4096);
#endif
		MSG("degree:%3d [%5d] %c%d.%06d : %c%d.%06d, err %c%d.%06d\n",
			i, r,
			s5d >= 0 ? '+' : '-',
			abs((int32_t)(((float)s5d/4096)*1000000)/1000000),
			abs((int32_t)(((float)s5d/4096)*1000000)%1000000),
			c >= 0 ? '+' : '-',
			abs((int32_t)(((float)c/4096)*1000000)/1000000), 
			abs((int32_t)(((float)c/4096)*1000000)%1000000), 
			err >= 0 ? '+' : '-',
			abs((int32_t)(((float)err/4096)*1000000)/1000000),
			abs((int32_t)(((float)err/4096)*1000000)%1000000));
	}
#ifdef PRINT_FLOAT
	MSG("cos err min: %f max: %f\n", (float)min/4096, (float)max/4096);
#endif
	MSG("cos err min: %c%d.%06d max: %c%d.%06d\n", 
		min >= 0 ? '+' : '-',
		abs((int32_t)(((float)min/4096)*1000000)/1000000), 
		abs((int32_t)(((float)min/4096)*1000000)%1000000), 
		max >= 0 ? '+' : '-',
		abs((int32_t)(((float)max/4096)*1000000)/1000000), 
		abs((int32_t)(((float)max/4096)*1000000)%1000000));

	return 0;
}
#endif

#ifdef SUPPORT_FP_SINCOS_APPROX_DEGREE
static int fp_sincos_degree(int argc, char * const argv[])
{
	int32_t r, val, s5d, ang, err;

	if (argc < 3)
		return -1;

	ang = strtol(argv[2], NULL, 10);
	if (ang < 0 || ang > 360)
		return -1;

	if (!strcmp(argv[1], "sv")) {
		r = __round((float)(ang * FP_DEGREE_RATIO)/1000);
		val = lround(4096*sin(2*M_PI * ang / 360));
		s5d = fpsin((int16_t)r);
		err = val - s5d;
	} else {
		r = __round((float)(ang * FP_DEGREE_RATIO)/1000);
		val = lround(4096*cos(2*M_PI * ang / 360));
		s5d = fpcos((int16_t)r);
		err = val - s5d;
	}

#ifdef PRINT_FLOAT
	MSG("degree:%3d [%5d] %+f : %+f, err %+f\n",
		ang, r, (float)s5d/4096, (float)val/4096, (float)err/4096);
#endif
	MSG("degree:%3d [%5d] %c%d.%06d : %c%d.%06d, err %c%d.%06d\n",
		ang, r,
		s5d >= 0 ? '+' : '-',
		abs((int32_t)(((float)s5d/4096)*1000000)/1000000),
		abs((int32_t)(((float)s5d/4096)*1000000)%1000000),
		val >= 0 ? '+' : '-',
		abs((int32_t)(((float)val/4096)*1000000)/1000000), 
		abs((int32_t)(((float)val/4096)*1000000)%1000000), 
		err >= 0 ? '+' : '-',
		abs((int32_t)(((float)err/4096)*1000000)/1000000),
		abs((int32_t)(((float)err/4096)*1000000)%1000000));

	return 0;
}
#endif

#ifdef SUPPORT_FP_SINCOS_APPROX_PERF
static int fp_sincos_perf(int argc UNUSED, char * const argv[] UNUSED)
{
	uint32_t st, time;
	volatile double ret = 0;

	/*
	 * Fixed Sine
	 */
	RUN_US(st);
    for (int n = 0; n <= PERF_COUNT; n++) {
		for (uint16_t i = 0; i <= 360; i++) {
			int32_t r = __round((float)(i * FP_DEGREE_RATIO)/1000);
			ret += fpsin((int16_t)r);
		}
    }
	END_US(st, time);
    MSG("fsin %d.%06d sec\n", time/1000000, time%1000000);

	/*
	 * Math Sine
	 */
	RUN_US(st);
    for (int n = 0; n <= PERF_COUNT; n++ ) {
		for (uint16_t i = 0; i <= 360; i++) {
			double r = (double)i * M_PI / 180;
	        ret += sin(r);
		}
    }
	END_US(st, time);
    MSG(" sin %d.%06d sec\n", time/1000000, time%1000000);

	/*
	 * Fixed Coine
	 */
	RUN_US(st);
    for (int n = 0; n <= PERF_COUNT; n++) {
		for (uint16_t i = 0; i <= 360; i++) {
			int32_t r = __round((float)(i * FP_DEGREE_RATIO)/1000);
			ret += fpcos((int16_t)r);
		}
    }
	END_US(st, time);
    MSG("fcos %d.%06d sec\n", time/1000000, time%1000000);

	/*
	 * Math Cosine
	 */
	RUN_US(st);
    for (int n = 0; n <= PERF_COUNT; n++ ) {
		for (uint16_t i = 0; i <= 360; i++) {
			double r = (double)i * M_PI / 180;
    	    ret += cos(r);
		}
    }
	END_US(st, time);

    MSG(" cos %d.%06d sec\n", time/1000000, time%1000000);

    return 0;
}
#endif

static struct testcase_t {
	char *cmd;
	int (*func)(int argc, char * const argv[]);
} fp_sincos_test[] = {
#ifdef SUPPORT_FP_SINCOS_APPROX_TESTING
	{
		.cmd = "test",
		.func = fp_sincos_testing,
	},
#endif
#ifdef SUPPORT_FP_SINCOS_APPROX_SIN
	{
		.cmd = "sin",
		.func = fp_sincos_sine,
	},
#endif
#ifdef SUPPORT_FP_SINCOS_APPROX_COS
	{
		.cmd = "cos",
		.func = fp_sincos_cosine,
	},
#endif
#ifdef SUPPORT_FP_SINCOS_APPROX_DEGREE
	{
		.cmd = "sv",
		.func = fp_sincos_degree,
	},
#endif
#ifdef SUPPORT_FP_SINCOS_APPROX_DEGREE
	{
		.cmd = "cv",
		.func = fp_sincos_degree,
	},
#endif

#ifdef SUPPORT_FP_SINCOS_APPROX_PERF
	{
		.cmd = "perf",
		.func = fp_sincos_perf,
	},
#endif
};

int main(int argc, char* const argv[])
{
	struct  testcase_t *test = fp_sincos_test;

	if (argc < 2)
		return -1;

	for (int i = 0; i < ARRAY_SIZE(fp_sincos_test); i++) {
		if (!strcmp(argv[1], test[i].cmd))
			return test[i].func(argc, argv);
	} 

    return 0;
}
