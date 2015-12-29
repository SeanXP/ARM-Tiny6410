/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < leds.h >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 	   < 2015/10/28 >
  > Description:		C语言实现LED灯控制, LED灯API设计
  						FriendlyARM - Tiny6410 裸机程序 

  LED对应的GPIO,要查看对应电路图。
  对应GPIO的控制寄存器和数据寄存器,要查看相应的芯片手册
  LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
  LED2 -> GPK5 -> GPKCON0[23:20] / GPKDAT[5]
  LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
  LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
  低电平点亮LED灯;
****************************************************************/


#ifndef __leds__
#define __leds__

#define LEDS_NUMBER		4

// {{{ LED - GPK
#define GPKCON0 (*(volatile unsigned long *)0x7F008800)
#define GPKDAT (*(volatile unsigned long *)0x7F008808)
//LED - 配置GPKCON[4-7]位为输出, (0001, output)
#define	GPK4_out	(1<<(4*4)) 	// bit16 = 1
#define	GPK5_out	(1<<(5*4))	// bit20 = 1
#define	GPK6_out	(1<<(6*4))	// bit24 = 1
#define	GPK7_out	(1<<(7*4))	// bit28 = 1
//GPKCON[4-7]的位掩码
#define	GPK4_mask	(0xf<<(4*4))	// bit[19:16] = 1111
#define	GPK5_mask	(0xf<<(5*4))
#define	GPK6_mask	(0xf<<(6*4))
#define	GPK7_mask	(0xf<<(7*4))
// LED配置（宏定义）
#define LED1_ON		(GPKDAT &= ~BIT4)
#define LED2_ON		(GPKDAT &= ~BIT5)
#define LED3_ON		(GPKDAT &= ~BIT6)
#define LED4_ON		(GPKDAT &= ~BIT7)
#define LED1_OFF	(GPKDAT |= BIT4)
#define LED2_OFF	(GPKDAT |= BIT5)
#define LED3_OFF	(GPKDAT |= BIT6)
#define LED4_OFF	(GPKDAT |= BIT7)
// }}}

// 配置LEDS的相关GPIO端口为输出;
void leds_init(void);

// 让第number个LED灯点亮;
void leds_ON(unsigned short number);

// 让第number个LED灯熄灭;
void leds_OFF(unsigned short number);

// 点亮所有LED灯
void leds_ON_all(void);

// 熄灭所有LED灯
void leds_OFF_all(void);

// 流水灯
// 参数: numbers, 循环次数;
void leds_flowing(unsigned int numbers);

// 二进制加法灯, 按照二进制数字闪烁;
// 参数: numbers, 循环次数;
void leds_binary(unsigned int numbers);

#endif
