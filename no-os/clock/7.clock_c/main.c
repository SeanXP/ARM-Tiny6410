/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < main.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 
  > Description:		C语言实现LED灯控制 - 流水灯(时钟配置)
  						FriendlyARM - Tiny6410 裸机程序 

  LED对应的GPIO,要查看对应电路图。
  对应GPIO的控制寄存器和数据寄存器,要查看相应的芯片手册
  LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
  LED2 -> GPK5 -> GPKCON0[23:20] / GPKDAT[5]
  LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
  LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
  低电平点亮LED灯;

****************************************************************/
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

// LED - GPK
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

// 延时函数定义;
void delay()
{
	volatile int i = 0x100000;
	while (i--);
}

int main()
{
	int number = 0;
	// LED对应管脚设为输出,这里都先使用掩码清零寄存器特定位, 在进行相关配置,可保证配置成功;
	GPKCON0 &= ~(GPK4_mask + GPK5_mask + GPK6_mask + GPK7_mask);
	GPKCON0 |=  (GPK4_out + GPK5_out + GPK6_out + GPK7_out);
	//开始默认关闭LED灯
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;

	while(1){
		number++;
		if(number > 0xf)
			number = 0;
		if(number & BIT0)
			LED1_ON;
		else
			LED1_OFF;
		if(number & BIT1)
			LED2_ON;
		else
			LED2_OFF;
		if(number & BIT2)
			LED3_ON;
		else
			LED3_OFF;
		if(number & BIT3)
			LED4_ON;
		else
			LED4_OFF;
		delay();	//根据实际效果调节延时
	}
	return 0;
}
