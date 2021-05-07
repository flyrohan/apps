#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "alu.h"

static struct TESTCASTE_t {
	const char *cmd;
	int (*op)(int x, int y);
	int (*fn)(int x, int y);
} alu [] = {
	{
		.cmd = "add",
		.op = __ADD, 
		.fn = alu_adder, 
	},
	{
		.cmd = "sub",
		.op = __SUB, 
		.fn = alu_substractor, 
	},
	{
		.cmd = "subn", /* substrator with not gate */
		.op = __SUB, 
		.fn = alu_substractor_ngate, 
	},
	{
		.cmd = "mul",
		.op = __MUL, 
		.fn = alu_multiplier, 
	},
	{
		.cmd = "div",
		.op = __DIV, 
		.fn = alu_divider, 
	},
	{
		.cmd = "mod",
		.op = __MOD,
		.fn = alu_modular, 
	},
};

static void usage(const char *app)
{
	printf("usage: %s\n\n", app);
	printf(" 'x' operator 'y'\n");
	printf("\t: operator is add/sub/subn/mul/div/mod\n");
	printf(" 'x' operator v\n");
	printf("\t: verify x/v with operator\n");
}

/*  
 * $> alu 'x' <op> 'y'  : calculate operator with 'x' an 'y'
 * $> alu 'x' <op> v	: verify with ragne 'x'
 */
int main(int argc, char const *argv[])
{
	int x, y, out, ret;
	bool verify = false;
	const char *cmd;

	if (argc < 4) {
		usage(argv[0]);
		return -1;
	}

	x = strtol(argv[1], NULL, 10);
	cmd = argv[2];
	if (!strcmp(argv[3], "v"))
		verify = true;
	else
		y = strtol(argv[3], NULL, 10);

	for (int i = 0; i < ARRAY_SIZE(alu); i++) {
		if (!strcmp(cmd, alu[i].cmd)) {
			if (verify == true) {
				int n;
				for (n = -x; n < x; n++) {
					if (n == 0 && (!strcmp(cmd, "div") | !strcmp(cmd, "mod")))
						continue;
					out = alu[i].op(x, n);
					ret = alu[i].fn(x, n);
					if (out != ret) {
						verify = false;
						break;
					}
				}
				printf("%s: %d %s %d = %d vs %d [0x%x:0x%x]\n",
					verify ? "PASS" : "FAIL", x, cmd, n, out, ret, out, ret);
			} else {
				out = alu[i].op(x, y);
				ret = alu[i].fn(x, y);
				printf("%s: %d %s %d = %d vs %d [0x%x:0x%x]\n",
					out == ret ? "PASS" : "FAIL", x, cmd, y, out, ret, out, ret);
			}
		}
	}

	return 0;
}
