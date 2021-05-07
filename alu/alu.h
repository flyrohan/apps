#ifndef __ALU__
#define __ALU__

int __ADD(int x, int y); 
int __SUB(int x, int y);
int __MUL(int x, int y);
int __DIV(int x, int y);
int __MOD(int x, int y);

int alu_adder(int x, int y);
int alu_substractor(int x, int y);
int alu_substractor_ngate(int x, int y);
int alu_multiplier(int x, int y);
int alu_divider(int x, int y);
int alu_modular(int x, int y);

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(int)(sizeof(x) / sizeof((x)[0]))
#endif

#endif
