/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < buzzer.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/21 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 - 裸机程序 - 蜂鸣器

	硬件连接:

	TinyADK-1312_sch.pdf: Buzzer1  - XpwmTOUT0/XCLKOUT/GPF14
****************************************************************/
#include "buzzer.h"

#define GPFCON (*(volatile unsigned int *)0x7F0080A0)
#define GPFDAT (*(volatile unsigned int *)0x7F0080A4)

// 00 = Input, 01 = Output, 10 = PWM ECLK, 11 = External Interrupt Group 4[13]
#define	GPF14_out	(1<<(14*2))	// [29:28] = 0b01
#define	GPF14_mask	(0x3<<(14*2))	// bit[29:28] = 0b11

#define BUZZER_ON	(GPFDAT |= 1<<14)
#define BUZZER_OFF	(GPFDAT &= ~(1<<14))
// 初始化蜂鸣器相关IO
void buzzer_init(void)
{
	// set GPF14 as output
	GPFCON &= ~(GPF14_mask);
	GPFCON |=  (GPF14_out);
}

// 开启蜂鸣器
void buzzer_ON(void)
{
	BUZZER_ON;
}

// 关闭蜂鸣器
void buzzer_OFF(void)
{
	BUZZER_OFF;
}
