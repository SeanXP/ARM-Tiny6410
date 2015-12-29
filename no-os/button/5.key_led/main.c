/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < main.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 
  > Description:		C语言实现按键控制LED灯
  FriendlyARM - Tiny6410 裸机程序 

  功能：按键控制LED灯，四个按键分别点亮LED1,LED2,LED3,LED4;

  LED对应的GPIO,要查看对应电路图。
  对应GPIO的控制寄存器和数据寄存器,要查看相应的芯片手册
  LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
  LED2 -> GPK5 -> GPKCON0[23:20] / GPKDAT[5]
  LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
  LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
  低电平点亮LED灯;

  Button对应的GPIO, 在Tiny6140底层扩展板(TinyADK 1312B版本) / 核心板(Tiny6410 1308):
  K1 -> XEINT16 -> 核心板P1排针-A53引脚 -> 核心板XEINT0 -> GPN0 -> GPNCON[1:0] / GPNDAT[0] 
  K2 -> XEINT17 -> 核心板P1排针-A54引脚 -> 核心板XEINT0 -> GPN1 -> GPNCON[3:2] / GPNDAT[1] 
  K3 -> XEINT18 -> 核心板P1排针-A55引脚 -> 核心板XEINT0 -> GPN2 -> GPNCON[5:4] / GPNDAT[2]
  K4 -> XEINT19 -> 核心板P1排针-A56引脚 -> 核心板XEINT0 -> GPN3 -> GPNCON[7:6] / GPNDAT[3]

  按键硬件连接上拉电阻，被按下, 对应IO被拉为低电平;

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

int main()
{
	// LED对应管脚设为输出,这里都先使用掩码清零寄存器特定位, 在进行相关配置,可保证配置成功;
	GPKCON0 &= ~(GPK4_mask + GPK5_mask + GPK6_mask + GPK7_mask);
	GPKCON0 |=  (GPK4_out + GPK5_out + GPK6_out + GPK7_out);
	//开始默认关闭LED灯
	LED1_OFF;
	LED2_OFF;
	LED3_OFF;
	LED4_OFF;
	// 按键对应管脚设为输入
	GPNCON &= ~(GPN0_mask + GPN1_mask + GPN2_mask + GPN3_mask);
	GPNCON |=  (GPN0_in + GPN1_in + GPN2_in + GPN3_in);

	//CPU轮询
	while(1){
		//若Kn为0(表示按下)，则令LEDn为0(表示点亮)

		//轮询法 判断按键是否按下
		if(BUTTON1)			//K1为高电平,没有被按下 
			LED1_OFF;
		else 
			LED1_ON;

		if(BUTTON2)			//K2为高电平,没有被按下 
			LED2_OFF;
		else 
			LED2_ON;

		if(BUTTON3)			//K3为高电平,没有被按下 
			LED3_OFF;
		else 
			LED3_ON;

		if(BUTTON4)			//K4为高电平,没有被按下 
			LED4_OFF;
		else 
			LED4_ON;
	}
	return 0;
}
