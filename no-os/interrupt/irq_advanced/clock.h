/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < clock.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/28 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 裸机程序 - clock 时钟配置(ACLK/MCLK) 
						Tiny6410 (1308版) - XTO/XTI - 12M HZ晶振

S3C6410: 
	时钟源:
		ARMCLK - CPU
		HCLK - AXI/AHB 总线外设(存储/中断/lcd等控制器)
		PCLK - APB 总线外设(看门狗，定时器，SD等)
	PPL : 
		1. 用于ARMCLK (for CPU); 
		2. 用于HCLK & PLK (AHB/APB总线设备);
		3. 用于外设 (供UART,IIS,IIC使用); 
****************************************************************/

#ifndef __clock__
#define __clock__


// {{{ ---------------- PLL Control Registers --------------------
// Control PLL locking period for APLL
#define APLL_LOCK (*((volatile unsigned long *)0x7E00F000))
// Control PLL locking period for MPLL
#define MPLL_LOCK (*((volatile unsigned long *)0x7E00F004))
// Control PLL locking period for EPLL
#define EPLL_LOCK (*((volatile unsigned long *)0x7E00F008))
//  设置各PLL的LOCK_TIME, 默认值0xffff 
//	设置PLL 后，时钟从FIN(PLL外部输入参考信号) 提升到目标频率时，需要一定的时间，即锁定时间。

// Control PLL output frequency for APLL
#define APLL_CON  (*((volatile unsigned long *)0x7E00F00C))

// APLL_CON: 
//		[31], ENABLE, PLL enable control (0: disable, 1: enable), Reset value: 0;
//		[25:16], MDIV, PLL M divide value, Reset value: 0x190 (400);
//		[13:8], PDIV, PLL P divide value, Reset value: 0x3 (3);
//		[2:0], SDIV, PLL S divide value, Reset value: 0x2 (2);
// ENABLE = 1; MDIV = 266; PDIV = 3; SDIV = 1;
// 
// (FIN / PDIV) = (FVCO / MDIV); FVCO / 2^SDIV = FOUT;
// FOUT = MDIV * FIN / (PDIV * (2^SDIV));
// FIN = 12M, Target FOUT = 200M, MDIV = 400, PDIV = 3, SDIV = 3
// FIN = 12M, Target FOUT = 400M, MDIV = 400, PDIV = 3, SDIV = 2
// FIN = 12M, Target FOUT = 533M, MDIV = 266, PDIV = 3, SDIV = 1
#define APLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))
// MDIV=[63,1023], PDIV=[1,63], SDIV=[0,5], FVCO = FIN * MDIV / PDIV = [800MHZ, 1600MHZ];
// FOUT=[40MHZ, 1600MHZ], FIN=[10MHZ, 20MHZ];

// Control PLL output frequency for MPLL
#define MPLL_CON  (*((volatile unsigned long *)0x7E00F010))
// MPLL_CON: 
//		[31], ENABLE, PLL enable control (0: disable, 1: enable), Reset value: 0;
//		[25:16], MDIV, PLL M divide value, Reset value: 0x214 (532);
//		[13:8], PDIV, PLL P divide value, Reset value: 0x6 (6);
//		[2:0], SDIV, PLL S divide value, Reset value: 0x3 (3);
// ENABLE = 1; MDIV = 266; PDIV = 3; SDIV = 1;
#define MPLL_CON_VAL  ((1<<31) | (266 << 16) | (3 << 8) | (1))

// Control PLL output frequency for EPLL
#define EPLL_CON0  (*((volatile unsigned long *)0x7E00F014))
#define EPLL_CON1  (*((volatile unsigned long *)0x7E00F018))

// -----------------------------------------------------------
// Others control register
#define OTHERS    (*((volatile unsigned long *)0x7e00f900))
// Set clock divider ratio
#define CLK_DIV0  (*((volatile unsigned long *)0x7E00F020))

// Select clock source, 时钟源选择, Reset Value: 0x0000_0000
// [0], APLL_SEL, Control MUXAPLL (0:FINAPLL, 1:FOUTAPLL);
// [1], MPLL_SEL, Control MUXMPLL (0:FINMPLL, 1:FOUTMPLL);
// [2], EPLL_SEL, Control MUXEPLL (0:FINEPLL, 1:FOUTEPLL);
#define CLK_SRC  (*((volatile unsigned long *)0x7E00F01C))

// Cacheable bus transaction selection register, Reset Value: 0x0000_0000
// [19], SYNC667, 0 : Normal Mode, 1 : Sync 667MHz Mode;
#define MISC_CON (*((volatile unsigned long *)0x7E00F838))
// }}}

// 分频系数RATIO
// 注释中计算式的前提: CLK_SRC = 1, 选择PLL; MISC_CON[19] = 0, Normal Mode;
#define ARM_RATIO    0  // ARMCLK 	= DOUTAPLL / (ARM_RATIO + 1)  	= 532/(0+1) = 532 MHz
#define MPLL_RATIO   0  // DOUTMPLL = MOUTMPLL / (MPLL_RATIO + 1)   = 532/(0+1) = 532  MHz
#define HCLKX2_RATIO 1  // HCLKX2 	= HCLKX2IN / (HCLKX2_RATIO + 1) = 532/(1+1) = 266  MHz
#define HCLK_RATIO   1  // HCLK 	= HCLKX2   / (HCLK_RATIO + 1)   = 266/(1+1) = 133  MHz
#define PCLK_RATIO   3  // PCLK   	= HCLKX2   / (PCLK_RATIO + 1)   = 266/(3+1) = 66.5 MHz

// 时钟初始化
void clock_init(void);

#endif
