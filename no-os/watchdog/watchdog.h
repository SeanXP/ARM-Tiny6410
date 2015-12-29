/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < watchdog.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/30 >
    > Last Changed: 
    > Description:		S3C6410 - 逻辑程序 - 看门狗定时器
****************************************************************/


#ifndef __watchdog__
#define __watchdog__

#define IRQ_WATCHDOG		// 开启看门狗定时器IRQ中断

// {{{ Watchdog Register Define
#define WDT_BASE			(0x7E004000)
// Watchdog timer control register
// [0], Reset enable/disable; 1:enable, 0:disable;
// [2], Interrupt generation; 1:enable, 0:disable;
// [4:3], Clock division factor; 00:16, 01:32, 10:64, 11:128;
// [5], Watchdog timer; 1:enable, 0:disable;
// [15:8], Prescaler value; 8-bit prescaler;
// 
// t_watchdog = 1/( PCLK / (Prescaler value + 1) / Division_factor )
#define WTCON				( *((volatile unsigned long *)(WDT_BASE+0x00)) )
// Watchdog timer data register
// [15:0], Counter reload value;
#define WTDAT				( *((volatile unsigned long *)(WDT_BASE+0x04)) )
// Watchdog timer count register;
// [15:0], The current count value of the watchdog timer;
#define WTCNT				( *((volatile unsigned long *)(WDT_BASE+0x08)) )
// Watchdog timer interrupt clear register;
// [0], Interrupt clear, Write any values clears the interrupt.
#define WTCLRINT			( *((volatile unsigned long *)(WDT_BASE+0x0C)) )
// }}}

#endif /* __watchdog__ */

// 定时器中断初始化(VIC寄存器配置), 此函数应在irq_init()函数中调用;
void watchdog_irq_init(void);

// 开启看门狗定时器
void watchdog_start();

//  关闭看门狗定时器
void watchdog_stop();

//  Watchdog Timer 中断服务函数
void watchdog_irq_handler(void);

// 看门狗定时器初始化, 并启动
// parameters: 
//            prescaler, 预分频系数;		8 bit, 1~255
// 			  divider, 固定分频通道;		00:16, 01:32, 10:64, 11:128		
//            wtdat, 初始计数值; 			
//			  interrupt, 是否中断;			0: 禁止中断; 1: 允许中断;
//			  reset, 是否复位;				0: 禁止复位; 1: 允许复位;
//
// t_watchdog = 1/( PCLK / (Prescaler value + 1) / Division_factor )
//
// watchdog_init(66, 0, 62500, 1, 0), prescaler = prescaler value + 1, divider value = 16; 
// 66MHZ / 66 / 16 = 62500HZ; WTDAT设置为62500, 则需要减62500次才能到0, 才能产生中断。即定时1s;
// interrupt = 1, 中断; reset = 0, 不复位;
//
void watchdog_init(unsigned long prescaler, unsigned long divider, unsigned long wtdat, 
		unsigned long interrupt, unsigned long reset);
