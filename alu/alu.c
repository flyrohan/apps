#include <stdio.h> 
#include "alu.h"

#ifdef  DEBUG
#define DBG			printf
#else
#define DBG
#endif

#define SIZE				(32)
#define SIGN_NEGATIVE		(0x80000000)
#define BI_COMPLEMENT(_v)	alu_adder(~_v, 1) 		

int __ADD(int x, int y)	{ return (x + y); }
int __SUB(int x, int y)	{ return (x - y); }
int __MUL(int x, int y)	{ return (x * y); }
int __DIV(int x, int y)	{ return (x / y); }
int __MOD(int x, int y)	{ return (x % y); }

/*
 *					________
 * <A> -------------|		|
 *				|	| XOR	| ---<xor>---		________
 *				|	|		|			|_______| 		|
 * <B> ---------|---|_______|			|		| XOR	|------------ <sum>
 *			|	|						|		|		|
 *	Cin ----------------------------------------|_______|
 * (CarryIn)|	|	________		|	|
 *			|	|___|		|		|	|		________
 *			|		| AND	|----	|	|_______|		|
 * 			|_______|		|	|	|			| AND	|
 *					|_______|	|	|___________|		|----
 *								|				|_______|	|	________
 *								|							|___|		|
 *								|								| OR	|------- <COut>
 *								|								|		|		(CarryOut)
 *								--------------------------------|_______|
 *
 */

int alu_adder(int x, int y)
{
	int out = 0, cin = 0;

	for (int i = 0; i < SIZE; i++) {
		int bix = (x & (1 << i));
		int biy = (y & (1 << i));
		int xor = (bix ^ biy);
		int sum = (xor ^ cin);

		cin = ((xor & cin) | (bix & biy)) << 1;
		out |= sum;
		DBG("%d + %d [%2d] 0x%x + 0x%x, xor:0x%x, sum:0x%x, cin:0x%x, out:0x%x\n",
			x, y, i, bix, biy, xor, sum, cin, out);
	}

	return out;
}

/*			
 *
 *
 *												  <COut>
 *													|
 *												____|____
 * <A> --------------------------------------->	|		|
 *								________		| ADDER	|------ SUM
 *								|		|		|		|
 * <B> -------------------------| 0		|----->	|		|
 *			|	________		|  MUX	|		|_______|
 * 			|___|		|_______| 1		|			|
 *				| NOT	|		|_______|			| <Cin>
 *				|_______| 			|_______________|
 *									|
 *									|
 *								 Select
 *
 *	if select is 0, run adder (a+b) 
 *	if select is 1, run substractor (a-b) and Cin is 1 for the 2'complement
 *
 */

int alu_substractor(int x, int y)
{
	return alu_adder(x, BI_COMPLEMENT(y));
}

/*
 *							________
 * <A> ---------------------|		|
 *				|			| XOR	| ---<xor>---			________
 *				|			|		|			|___________| 		|
 * <B> ---------|----------	|_______|			|			| XOR	|------------ <Diff>
 *			|	|								|			|		|
 *	Bin ----------------------------------------------------|_______|
 * (BorrowIn|	|			________		|	|
 *			|	---[ NOT]---|		|		|	|			________
 *			|				| AND	|----	|	----[NOT]---|		|
 * 			|_______________|		| 	|	|				| AND	|
 *							|_______|	|	|_______________|		|----
 *										|					|_______|	|	________
 *										|								|___|		|
 *										|									| OR	|------- <Bout>
 *										|									|		|		(BorrowOut)
 *										------------------------------------|_______|
 *
 */

int alu_substractor_ngate(int x, int y)
{
	int out = 0, bin = 0;

	for (int i = 0; i < SIZE; i++) {
		int bix = (x & (1 << i));
		int biy = (y & (1 << i));
		int xor = (bix ^ biy);
		int sum = (xor ^ bin);

		bin = ((~xor & bin) | (~bix & biy)) << 1;
		out |= sum;
		DBG("%d - %d [%2d] 0x%x - 0x%x, xor:0x%x, sum:0x%x, bin:0x%x, out:0x%x\n",
			x, y, i, bix, biy, xor, sum, bin, out);
	}

	return out;
}

/*
 *
 *		 			x31 x... x0
 *  			X	y31 y... y0
 * 				---------------	
 *  			x31y0 x... x0y0
 *    		x31y1 x... x0y1
 *    	x31y2 x... x0y2
 *  +  	....
 *  ----------------------------
 * 		P32 P............... P0
 *
 */

int alu_multiplier(int x, int y)
{
	int out = 0;

	for (int i = 0; i < SIZE; i++) {
		if (y & (1 << i))
			out = alu_adder(out, (x << i));
	}

	return out;
}


static int first_set(int x)
{
	for (int i = (SIZE - 1); i >= 0 ; i--) {
		if (x & (1 << i))
			return i;
	}
	return 0;
}
/*
 * 0111 / 10
 *
 *		  11	   
 * 		-----------
 *  10 | 111 
 *		 10  ---  11 
 *		  11
 *		  10 ---- 1
 *	
 *
 */

int alu_divider(int x, int y)
{
	int d = x, n = y;
	int out = 0;

	if (y == 0) {
		printf("Zero division error !!!\n");
		return 0;
	}

	if (x & SIGN_NEGATIVE)
		d = BI_COMPLEMENT(x);

	if (y & SIGN_NEGATIVE)
		n = BI_COMPLEMENT(y);

	for (int i = first_set(x); i >= 0; i--) {
		int dlt = alu_substractor((d >> i), n);
		if (!(dlt & SIGN_NEGATIVE)) {
			d = (dlt << i) | (d & alu_substractor((1 << i), 1));
			out = alu_adder(out, (1 << i));
			if (d == 0)
				break;

			DBG("%d / %d [%2d] dlt:0x%x, d:0x%x, out:0x%x\n",
				x, y, i, dlt, d, out);
		}
	}

	return ((x | y ) & SIGN_NEGATIVE) ? BI_COMPLEMENT(out) : out;
}

int alu_modular(int x, int y)
{
	int d = x, n = y;

	if (y == 0) {
		printf("Zero division error !!!\n");
		return 0;
	}

	if (x & SIGN_NEGATIVE)
		d = BI_COMPLEMENT(x);

	if (y & SIGN_NEGATIVE)
		n = BI_COMPLEMENT(y);

	for (int i = first_set(x); i >= 0; i--) {
		int dlt = alu_substractor((d >> i), n);
		if (!(dlt & SIGN_NEGATIVE)) {
			d = (dlt << i) | (d & alu_substractor((1 << i), 1));
			if (d == 0)
				break;

			DBG("%d / %d [%2d] dlt:0x%x, d:0x%x\n",
				x, y, i, dlt, d);
		}
	}

	return d;
}

