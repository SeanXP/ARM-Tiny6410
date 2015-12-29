/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < print.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/10/30 >
  > Last Changed: 
  > Description:
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>

char regs[32][8] = 
{
	"M0", "M1", "M2",	"M3",	"M4",	"T", "F",	"I", /* 0-7 */
	"A", "E", " ", " ", " ", "CV", " ", " ", /* 8-15 */
	"|", "G", "E", "|", " ", " ", " ", " ", /* 16-23 */
	"J", " ", " ", "Q", "V", "C", "Z", "N"  /* 24-31 */
};

// 格式化打印寄存器的值(32位一位一位打印)
void format(char *regname, int reg)
{
	int i = 0;

	if(!reg)
		return;

	printf("%3s", regname);
	for(i = 31; i >= 0; i--)
		printf("%3x", reg & (1 << i) ? 1 : 0);
	printf("\n");
}

void print(int r0, int r1, int r2)
{
	int i = 0;

	printf(" General out:\n");
	printf(" Hex\tr0 0x%08x\tr1 0x%08x\tr2 0x%08x\n", r0, r1, r2);
	printf(" Oct\tr0 %8d\tr1 %8x\tr2 %8d\n\n", r0, r1, r2);

	if(r0 || r1 || r2)
	{
		printf(" Format out:\n   ");		
		for(i = 31; i >= 0; i--)
			printf("%3d", i);
		printf("\n   ");
		for(i = 31; i >= 0; i--)
			printf("%3s", regs[i]);
		printf("\n");
	}

	format("r0", r0);
	format("r1", r1);
	format("r2", r2);
}

void quit(int err)
{
	exit(err);
}
