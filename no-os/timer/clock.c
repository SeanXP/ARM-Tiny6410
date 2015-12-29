/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < clock.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/07/31 >
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
#include "clock.h"

// {{{ 时钟初始化
void clock_init(void)
{	
	/* 1. 设置各PLL的LOCK_TIME,使用默认值 */
	//	设置PLL 后，时钟从FIN(PLL外部输入参考信号) 提升到目标频率时，需要一定的时间，即锁定时间。
	//	这里我们设置为最大值(保证足够时间进行PLL)。
	APLL_LOCK = 0xffff;			// APLL_LOCK，供cpu使用 
	MPLL_LOCK = 0xffff;			// MPLL_LOCK，供AHB(存储/中断/lcd等控制器)/APB(看门狗，定时器，SD等)总线上的设备使用
	EPLL_LOCK = 0xffff;			// EPLL_LOCK，供UART,IIS,IIC使用 

	/* 2. 设置为异步模式(Asynchronous mode) */
	//《linux installation for u-boot》3.7中：用MPLL作为HCLK和PCLK的Source是异步(ASYNC)模式,用APLL是同步(SYNC)模式
	// S3C6410 硬性规定: 用MPLL作为HCLK和PCLK的Source,需设置为异步(ASYNC)模式;
	//					 若用APLL,则设置为同步(SYNC)模式；
	// OTHERS bit[7] - SYNCMODE, SYNCMODEREQ to ARM; 0: Asynchronous mode, 1: Synchronous mode	
	// OTHERS bit[6] - SYNCMUXSEL, SYS CLOCK SELECT IN CMU; 
	OTHERS &= ~0xc0;			// 1100 0000 b; 即bit[7:6] 清零;						
	// OTHERS bit[11:8] - SYNCACK, SYNC mode acknowledge (Read Only);
	while ((OTHERS & 0xf00) != 0);	// 循环等待, SYNCACK为0;

	/* 3. 设置分频系数 */
	// S3C6410有三个分频系数寄存器;
	CLK_DIV0 = (ARM_RATIO) | (MPLL_RATIO << 4) | (HCLK_RATIO << 8) | (HCLKX2_RATIO << 9) | (PCLK_RATIO << 12);

	/* 4. 设置PLL,放大时钟 */	
	APLL_CON = APLL_CON_VAL;  
	MPLL_CON = MPLL_CON_VAL;  

	/* 5. 选择PLL的输出作为时钟源 */
	CLK_SRC = 0x03; // bit[1:0] = 0x11; APLL_SEL = 1, MPLL_SEL = 1; 
	// bit[2] = 0x0; EPLL_SEL = 0;
}//}}}
