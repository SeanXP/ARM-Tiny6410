/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < key.h >
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


#ifndef __key__
#define __key__

#define IRQ_KEY	// 开启外部按键中断

// {{{ Button - GPN
#define GPNCON (*(volatile unsigned long *)0x7F008830)
#define GPNDAT (*(volatile unsigned long *)0x7F008834)
//配置按键对应的GPNCON[0:3]为输入, GPNCON[1:0],[3:2],[5:4],[7,6]
#define GPN0_in			(0<<(0*2))
#define GPN1_in     	(0<<(1*2))
#define GPN2_in     	(0<<(2*2))
#define GPN3_in     	(0<<(3*2))
//配置按键对应的GPNCON[0:3]为中断引脚(0b10)
// External interrupt Group 0: 0 ~ 3, GPN0~3, INT_EINT0;
#define GPN0_int		(2<<(0*2))
#define GPN1_int     	(2<<(1*2))
#define GPN2_int     	(2<<(2*2))
#define GPN3_int     	(2<<(3*2))
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
// }}}

// 配置key的相关GPIO端口为输入;
void key_init(void);

// 返回按键的值(0 / 1);
// 在Tiny6410中，低电平表示被按下;
char key_1();
char key_2();
char key_3();
char key_4();

// 配置key的相关GPIO端口为中断引脚;
void key_irq_init(void);

// Key1~4外部中断服务函数 - VIC0 - INT_EINT0 - External interrupt Group 0: 0 ~ 3
void key_irq_handler(void);

#endif


