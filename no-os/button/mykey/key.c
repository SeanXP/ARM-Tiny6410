/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < key.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/08/12 >
    > Last Changed: 
	> Description:		C语言实现按键控制, 按键API设计
  						FriendlyARM - Tiny6410 裸机程序 

  Button对应的GPIO, 在Tiny6140底层扩展板(TinyADK 1312B版本) / 核心板(Tiny6410 1308):
  K1 -> XEINT16 -> 核心板P1排针-A53引脚 -> 核心板XEINT0 -> GPN0 -> GPNCON[1:0] / GPNDAT[0] 
  K2 -> XEINT17 -> 核心板P1排针-A54引脚 -> 核心板XEINT0 -> GPN1 -> GPNCON[3:2] / GPNDAT[1] 
  K3 -> XEINT18 -> 核心板P1排针-A55引脚 -> 核心板XEINT0 -> GPN2 -> GPNCON[5:4] / GPNDAT[2]
  K4 -> XEINT19 -> 核心板P1排针-A56引脚 -> 核心板XEINT0 -> GPN3 -> GPNCON[7:6] / GPNDAT[3]

  按键硬件连接上拉电阻，被按下, 对应IO被拉为低电平;

****************************************************************/

#include "key.h"

// BIT位掩码宏定义
#define	BIT0		(1<<0)
#define	BIT1		(1<<1)
#define	BIT2		(1<<2)
#define	BIT3		(1<<3)
#define	BIT4		(1<<4)
#define	BIT5		(1<<5)
#define	BIT6		(1<<6)
#define	BIT7		(1<<7)
#define	BIT8		(1<<8)
#define	BIT9		(1<<9)
#define	BIT10		(1<<10)
#define	BIT11		(1<<11)
#define	BIT12		(1<<12)
#define	BIT13		(1<<13)
#define	BIT14		(1<<14)
#define	BIT15		(1<<15)

// Button - GPN
#define GPNCON (*(volatile unsigned long *)0x7F008830)
#define GPNDAT (*(volatile unsigned long *)0x7F008834)
//配置按键对应的GPNCON[0:3]为输出, GPNCON[1:0],[3:2],[5:4],[7,6]
#define GPN0_in			(0<<(0*2))
#define GPN1_in     	(0<<(1*2))
#define GPN2_in     	(0<<(2*2))
#define GPN3_in     	(0<<(3*2))
//GPNCON的位掩码
#define GPN0_mask		(0x3<<(0*2))
#define GPN1_mask		(0x3<<(1*2))
#define GPN2_mask		(0x3<<(2*2))
#define GPN3_mask		(0x3<<(3*2))
//GPNDAT 位掩码, 低电平表示按键被按下
#define	BUTTON1		(GPNDAT & BIT0)
#define	BUTTON2		(GPNDAT & BIT1)
#define	BUTTON3		(GPNDAT & BIT2)
#define	BUTTON4		(GPNDAT & BIT3)

// 配置key的相关GPIO端口为输入;
void key_init(void)
{
	GPNCON &= ~(GPN0_mask + GPN1_mask + GPN2_mask + GPN3_mask);
	GPNCON |=  (GPN0_in + GPN1_in + GPN2_in + GPN3_in);
}

//返回按键的值;
char key_1()
{
	return (BUTTON1);
}
char key_2()
{
	return (BUTTON2);
}
char key_3()
{
	return (BUTTON3);
}
char key_4()
{
	return (BUTTON4);
}
