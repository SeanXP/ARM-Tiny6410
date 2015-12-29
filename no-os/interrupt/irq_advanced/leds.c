/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < leds.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 
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

#include "leds.h"
#include "common.h"

// {{{ 简单延时函数定义;
static void delay()
{
	volatile int i = 0x100000;
	while (i--);
} //}}}

// {{{ 配置LEDS的相关GPIO端口为输出;
void leds_init(void)
{
	// LED对应管脚设为输出,这里都先使用掩码清零寄存器特定位, 在进行相关配置,可保证配置成功;
	GPKCON0 &= ~(GPK4_mask + GPK5_mask + GPK6_mask + GPK7_mask);
	GPKCON0 |=  (GPK4_out + GPK5_out + GPK6_out + GPK7_out);
} //}}}

// {{{ 让第number个LED灯点亮;
void leds_ON(unsigned short number)
{
	if(number <= LEDS_NUMBER)
	{
		switch(number)
		{
			case 1:
				LED1_ON; 
				break;
			case 2:
				LED2_ON;
				break;
			case 3:
				LED3_ON;
				break;
			case 4:
				LED4_ON;
				break;
		}	
	}
} //}}}

// {{{ 让第number个LED灯熄灭;
void leds_OFF(unsigned short number)
{
	if(number <= LEDS_NUMBER)
	{
		switch(number)
		{
			case 1:
				LED1_OFF; 
				break;
			case 2:
				LED2_OFF;
				break;
			case 3:
				LED3_OFF;
				break;
			case 4:
				LED4_OFF;
				break;
		}	
	}
} //}}}

// {{{ 点亮所有LED灯
void leds_ON_all(void)
{
	unsigned short number = 1;
	for(number = 1; number <= LEDS_NUMBER; number++)
	{
		leds_ON(number);
	}
} // }}}

// {{{ 熄灭所有LED灯
void leds_OFF_all(void)
{
	unsigned short i = 1;
	for(i = 1; i <= LEDS_NUMBER; i++)
	{
		leds_OFF(i);
	}
} //}}}

// {{{ 流水灯
// 参数: numbers, 次数
void leds_flowing(unsigned int numbers)
{
	unsigned short i = 1;
	while(numbers--)
	{
		for(i = 1; i <= LEDS_NUMBER; i++)
		{
			leds_OFF_all();
			leds_ON(i);
			delay();
		}
	}
} // }}}

// {{{ 二进制加法灯, 按照二进制数字闪烁;
void leds_binary(unsigned int numbers)
{
	unsigned short i = 0;

	while(numbers){
		i++;
		if(i > 0xf)	// 这个判断值为 2^(LEDS_NUMBER); 这里默认4个LED灯;
	   	{
			i = 0;	
			numbers = numbers - 1;
		}

		if(i & BIT0)
			LED1_ON;
		else
			LED1_OFF;

		if(i & BIT1)
			LED2_ON;
		else
			LED2_OFF;

		if(i & BIT2)
			LED3_ON;
		else
			LED3_OFF;

		if(i & BIT3)
			LED4_ON;
		else
			LED4_OFF;
		delay();
	}
} // }}}
