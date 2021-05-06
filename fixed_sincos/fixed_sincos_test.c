// Written in 2012 by Joe Tsai <joetsai@digital-static.net>
//
// ===================================================================
// The contents of this file are dedicated to the public domain. To
// the extent that dedication to the public domain is not available,
// everyone is granted a worldwide, perpetual, royalty-free,
// non-exclusive license to exercise all rights associated with the
// contents of this file for any purpose whatsoever.
// No rights are reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ===================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "fixed_sincos.h"

#include <time.h>
#define RUN_US(_s) 		do { _s = clock(); } while(0)
#define END_US(_s, _t) 	do { _t = clock() - _s; } while (0)
#define PRINT_FLOAT
#define MSG(m...)		printf(m)

#define SUPPORT_FIXED_SINCOS_DEMO
#define SUPPORT_FIXED_SINCOS_APPROX
#define SUPPORT_FIXED_SINCOS_APPROX_SIN
#define SUPPORT_FIXED_SINCOS_APPROX_COS
#define SUPPORT_FIXED_SINCOS_PERF
#define SUPPORT_FIXED_SINCOS_STAT

#define PERF_COUNT		(360 * 100000)

#ifdef SUPPORT_FIXED_SINCOS_DEMO
static int fixed_sincos_demo(int argc __attribute__((unused)),
							 char * const argv[] __attribute__((unused)))
{
    // Simple demo program prints out the 4096 samples of the approximate
    // sine and cosine waves
    MSG("count   angle  sine       cosine  \n");
    for (int i = 0; i < 4096; i++) {
        int64_t angle = float_to_fixed(1.0/4096.0*i,20);
        int64_t sin_val = sine(angle);
        int64_t cos_val = cosine(angle);
#ifdef PRINT_FLOAT
        MSG(
            "[%4d] %6ld, %+0.6f, %+0.6f\n",
            i, angle,
			fixed_to_float(sin_val,17),
            fixed_to_float(cos_val,17));
#endif
        MSG(
            "[%4d] %6ld, %c%d.%06d, %c%d.%06d\n",
            i, angle,
			sin_val >= 0 ? '+' : '-',
			abs((int32_t)(fixed_to_float(sin_val,17) * 1000000)/1000000),
			abs((int32_t)(fixed_to_float(sin_val,17) * 1000000)%1000000),
			cos_val >= 0 ? '+' : '-',
			abs((int32_t)(fixed_to_float(cos_val,17) * 1000000)/1000000),
			abs((int32_t)(fixed_to_float(cos_val,17) * 1000000)%1000000));
    }

	return 0;
}
#endif

#ifdef SUPPORT_FIXED_SINCOS_APPROX
static int fixed_sincos_approximator(int argc __attribute__((unused)),
									 char * const argv[] __attribute__((unused)))
{
	float sin_max = 0, cos_max = 0;
	int sin_ang = 0, cos_ang = 0;

	MSG("angle    fsin    sin       diff              fcos   cos       diff\n");
    for (int i = 0; i <= 360; i+= 1 ) {
        int64_t angle = float_to_fixed(((float)i/360), 20);
        float fsin_val = (float)fixed_to_float(sine(angle), 17);
        float fcos_val = (float)fixed_to_float(cosine(angle), 17);
		float sin_val = (float)sin((float)i * M_PI / 180);
		float cos_val = (float)cos((float)i * M_PI / 180);
		float sin_dif = (float)fabs(fsin_val - sin_val);
		float cos_dif = (float)fabs(fcos_val - cos_val);
		
		if (sin_max < sin_dif) {
			sin_max = sin_dif;
			sin_ang = i;
		}

		if (cos_max < cos_dif) {
			cos_max = cos_dif;
			cos_ang = i;
		}

#ifdef PRINT_FLOAT
        MSG(
            "[%3d] %+0.6f:%+0.6lf [%0.9f], %+0.6f:%+0.6lf [%0.9f]\n",
            i, 
			fsin_val, sin_val, sin_dif,
            fcos_val, cos_val, cos_dif);
#endif
        MSG(
            "      %c%d.%06d:%c%d.%06d [%d.%09d], %c%d.%06d:%c%d.%06d [%d.%09d]\n",
			fsin_val >= 0 ? '+' : '-',
			abs((int32_t)(fsin_val * 1000000)/1000000), 
			abs((int32_t)(fsin_val * 1000000)%1000000), 
			sin_val >= 0 ? '+' : '-',
			abs((int32_t)(sin_val * 1000000)/1000000), 
			abs((int32_t)(sin_val * 1000000)%1000000), 
			abs((int32_t)(sin_dif * 1000000000)/1000000000), 
			abs((int32_t)(sin_dif * 1000000000)%1000000000), 
			fcos_val >= 0 ? '+' : '-',
			abs((int32_t)(fcos_val * 1000000)/1000000), 
			abs((int32_t)(fcos_val * 1000000)%1000000), 
			cos_val >= 0 ? '+' : '-',
			abs((int32_t)(cos_val * 1000000)/1000000), 
			abs((int32_t)(cos_val * 1000000)%1000000), 
			abs((int32_t)(cos_dif * 1000000000)/1000000000), 
			abs((int32_t)(cos_dif * 1000000000)%1000000000));
    }

#ifdef PRINT_FLOAT
    MSG("max diff sin: %3d: %0.12f, cos: %3d: %0.12f\n",
		sin_ang, sin_max, cos_ang, cos_max); 
#endif
    MSG("max diff sin: %3d: %d.%09d, cos: %3d: %d.%09d\n",
		sin_ang, 
		abs((int32_t)(sin_max * 1000000000)/1000000000), 
		abs((int32_t)(sin_max * 1000000000)%1000000000), 
		cos_ang, 
		abs((int32_t)(cos_max * 1000000000)/1000000000), 
		abs((int32_t)(cos_max * 1000000000)%1000000000)); 

	return 0;
}
#endif

#ifdef SUPPORT_FIXED_SINCOS_APPROX_SIN
static int fixed_sin_approximator(int argc, char * const argv[])
{
	float angle;

	if (argc < 3) {
		MSG("Usage: %s sin ANGLE\n",argv[0]);
        return -1;
	}

	angle = (float)atof(argv[2]);

#ifdef PRINT_FLOAT
	MSG(
		"sine(2*M_PI*%g) sine(%g) = %+0.6f\n",
        angle, (2 * M_PI * angle),
        fixed_to_float(sine(float_to_fixed(angle, 20)), 17));
#endif
	MSG(
		"sine(2*M_PI*%d.%03d) sine(%d.%06d) = %c%d.%06d\n",
        (int32_t)(angle*1000)/1000,
		(int32_t)(angle*1000)%1000,
		(int32_t)((2 * M_PI * angle)*1000000)/1000000,
		(int32_t)((2 * M_PI * angle)*1000000)%1000000,
		fixed_to_float(sine(float_to_fixed(angle, 20)), 17) >= 0 ? '+' : '-',
        abs((int32_t)(fixed_to_float(sine(float_to_fixed(angle, 20)), 17) * 1000000)/1000000),
        abs((int32_t)(fixed_to_float(sine(float_to_fixed(angle, 20)), 17) * 1000000)%1000000));

	return 0;
}
#endif

#ifdef SUPPORT_FIXED_SINCOS_APPROX_COS
static int fixed_cos_approximator(int argc, char * const argv[])
{
	float angle;

	if (argc < 3) {
		MSG("Usage: %s cos ANGLE\n",argv[0]);
        return -1;
	}

	angle = (float)atof(argv[2]);

#ifdef PRINT_FLOAT
	MSG(
        "cosine(2*M_PI*%g) cosine(%g) =  %+0.6f\n",
        angle, (2 * M_PI * angle),
        fixed_to_float(cosine(float_to_fixed(angle, 20)), 17));
#endif
	MSG(
		"cosine(2*M_PI*%d.%03d) cosine(%d.%06d) = %c%u.%06u\n",
        (int32_t)(angle*1000)/1000,
		(int32_t)(angle*1000)%1000,
		(int32_t)((2 * M_PI * angle)*1000000)/1000000,
		(int32_t)((2 * M_PI * angle)*1000000)%1000000,
		fixed_to_float(cosine(float_to_fixed(angle, 20)), 17) >= 0 ? '+' : '-',
        abs((int32_t)(fixed_to_float(cosine(float_to_fixed(angle, 20)), 17) * 1000000)/(int32_t)1000000),
        abs((int32_t)(fixed_to_float(cosine(float_to_fixed(angle, 20)), 17) * 1000000)%(int32_t)1000000));

	return 0;
}
#endif

#ifdef SUPPORT_FIXED_SINCOS_STAT
static int fixed_sincos_stat(int argc __attribute__((unused)),
							 char * const argv[] __attribute__((unused)))
{
    // Compute the estimation error across all possible values
    size_t i;
    int domain = 1 << 20;
    double fdomain = (double)domain;

    double* sin_errs = calloc(domain, sizeof(double));
    double* cos_errs = calloc(domain, sizeof(double));
    for (i = 0; i < (size_t)domain; i++) {
        // Compute the real value from C math library
        double float_angle = 2*M_PI*1.0/fdomain*i;
        double float_sin = sin(float_angle);
        double float_cos = cos(float_angle);

        // Compute the estimated value fixed-point methods library
        int64_t fixed_angle = float_to_fixed(1.0/fdomain*i,20);
        double fixed_sin = fixed_to_float(sine(fixed_angle),17);
        double fixed_cos = fixed_to_float(cosine(fixed_angle),17);

        sin_errs[i] = fabs(float_sin - fixed_sin);
        cos_errs[i] = fabs(float_cos - fixed_cos);
    }

    // Compute average
    double sin_err_sum = 0.0;
    double cos_err_sum = 0.0;
    for (i = 0; i < (size_t)domain; i++) {
        sin_err_sum += sin_errs[i];
        cos_err_sum += cos_errs[i];
    }
    double sin_err_avg = sin_err_sum / fdomain;
    double cos_err_avg = cos_err_sum / fdomain;

    // Compute standard deviation
    double sin_err_sumsqr = 0.0;
    double cos_err_sumsqr = 0.0;
    for (i = 0; i < (size_t)domain; i++) {
        sin_err_sumsqr += pow(sin_errs[i] - sin_err_avg, 2.0);
        cos_err_sumsqr += pow(cos_errs[i] - cos_err_avg, 2.0);
    }
    double sin_err_stdev = sqrt(sin_err_sumsqr / fdomain);
    double cos_err_stdev = sqrt(cos_err_sumsqr / fdomain);

    // Compute the maximum error
    double sin_err_max = 0.0;
    double cos_err_max = 0.0;
    for (i = 0; i < (size_t)domain; i++) {
        sin_err_max = fmax(sin_err_max, sin_errs[i]);
        cos_err_max = fmax(cos_err_max, cos_errs[i]);
    }

    MSG("sine\n");
#ifdef PRINT_FLOAT
    MSG("\tavg:   %0.12f\n", sin_err_avg);
#endif
    MSG("\tavg:   %d.%09d\n",
		(int32_t)(sin_err_avg*1000000000)/1000000000,
		(int32_t)(sin_err_avg*1000000000)%1000000000);
#ifdef PRINT_FLOAT
    MSG("\tstdev: %0.12f\n", sin_err_stdev);
#endif
    MSG("\tstdev: %d.%09d\n",
		(int32_t)(sin_err_stdev*1000000000)/1000000000,
		(int32_t)(sin_err_stdev*1000000000)%1000000000);
#ifdef PRINT_FLOAT
    MSG("\tmax:   %0.12f\n", sin_err_max);
#endif
    MSG("\tmax:   %d.%09d\n",
		(int32_t)(sin_err_max*1000000000)/1000000000,
		(int32_t)(sin_err_max*1000000000)%1000000000);

    MSG("cosine\n");
#ifdef PRINT_FLOAT
    MSG("\tavg:   %0.12f\n", cos_err_avg);
#endif
    MSG("\tavg:   %d.%09d\n",
		(int32_t)(cos_err_avg*1000000000)/1000000000,
		(int32_t)(cos_err_avg*1000000000)%1000000000);
#ifdef PRINT_FLOAT
    MSG("\tstdev: %0.12f\n", cos_err_stdev);
#endif
    MSG("\tstdev: %d.%09d\n",
		(int32_t)(cos_err_stdev*1000000000)/1000000000,
		(int32_t)(cos_err_stdev*1000000000)%1000000000);
#ifdef PRINT_FLOAT
    MSG("\tmax:   %0.12f\n", cos_err_max);
#endif
   MSG("\tmax:   %d.%09d\n",
		(int32_t)(cos_err_max*1000000000)/1000000000,
		(int32_t)(cos_err_max*1000000000)%1000000000);

    free(sin_errs);
    free(cos_errs);

	return 0;
}
#endif

#ifdef SUPPORT_FIXED_SINCOS_PERF
static int fixed_sincos_perf(int argc __attribute__((unused)),
							 char * const argv[] __attribute__((unused)))
{
	uint64_t st, time;
	volatile double ret = 0;

	/*
	 * Fixed Sine
	 */
	RUN_US(st);
    for (int i = 0; i <= PERF_COUNT; i+= 1 ) {
		int64_t angle = float_to_fixed(((float)i/360), 20);
        ret += fixed_to_float(sine(angle), 17);
    }
	END_US(st, time);
    MSG("fsin %ld.%06ld sec\n", time/1000000, time%1000000);

	/*
	 * Math Sine
	 */
	RUN_US(st);
    for (int i = 0; i <= PERF_COUNT; i+= 1 ) {
		double radian = (double)i * M_PI / 180;
        ret += sin(radian);
    }
	END_US(st, time);
    MSG(" sin %ld.%06ld sec\n", time/1000000, time%1000000);
    
	/*
	 * Fixed Cosine
	 */
	RUN_US(st);
    for (int i = 0; i <= PERF_COUNT; i+= 1 ) {
		int64_t angle = float_to_fixed(((float)i/360), 20);
        ret += fixed_to_float(cosine(angle), 17);
    }
	END_US(st, time);
    MSG("fcos %ld.%06ld sec\n", time/1000000, time%1000000);

	/*
	 * Math Cosine
	 */
	RUN_US(st);
    for (int i = 0; i <= PERF_COUNT; i+= 1 ) {
		double radian = (double)i * M_PI / 180;
        ret += cos(radian);
    }
	END_US(st, time);

    MSG(" cos %ld.%06ld sec\n", time/1000000, time%1000000);

	return 0;
}
#endif

static struct testcase_t {
	char *cmd;
	int (*func)(int argc, char * const argv[]);
} fixed_sincos_test[] = {
#ifdef SUPPORT_FIXED_SINCOS_DEMO
	{ 
		.cmd = "demo",
		.func = fixed_sincos_demo,
	},
#endif
#ifdef SUPPORT_FIXED_SINCOS_APPROX
	{
		.cmd = "approx",
		.func = fixed_sincos_approximator,
	},
#endif
#ifdef SUPPORT_FIXED_SINCOS_APPROX_SIN
	{ 
		.cmd = "sin",
		.func = fixed_sin_approximator,
	},
#endif
#ifdef SUPPORT_FIXED_SINCOS_APPROX_COS
	{ 
		.cmd = "cos",
		.func = fixed_cos_approximator,
	},
#endif
#ifdef SUPPORT_FIXED_SINCOS_PERF
	{
		.cmd = "perf",
		.func = fixed_sincos_perf,
	},
#endif
#ifdef SUPPORT_FIXED_SINCOS_STAT
	{ 
		.cmd = "stat",
		.func = fixed_sincos_stat,
	},
#endif
} ;

/*
 * main <mode> <angle>
 *
 * mode : demo, approx, perf, sin, cos, stat
 *
 */
int main(int argc, char* const argv[])
{
	struct  testcase_t *test = fixed_sincos_test;

	for (int i = 0; i < ARRAY_SIZE(fixed_sincos_test); i++) {
		if (!strcmp(argv[1], test[i].cmd))
			return test[i].func(argc, argv);
	} 

    return 0;
}


