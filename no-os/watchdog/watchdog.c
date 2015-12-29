/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < watchdog.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/30 >
    > Last Changed: 
    > Description:		S3C6410 - 逻辑程序 - 看门狗定时器
****************************************************************/

#include "watchdog.h"
#include "irq.h"
#include "uart.h"

// {{{ 定时器中断初始化(VIC寄存器配置), 此函数应在irq_init()函数中调用;
void watchdog_irq_init(void)
{
	// VIC0 - No.26 - INT_WDT - Watchdog timer interrupt
	// 设置中断类型
	VIC0INTSELECT &= ~(1 << 26); // 0, IRQ; 1, FIQ;

	// 设置ISR地址
	VIC0VECTADDR(26) = (unsigned int)watchdog_irq_handler; 

	VIC0INTENABLE |= (1 << 26);	// 在中断控制器里使能中断

}// }}}

// {{{ 看门狗定时器初始化, 并启动
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
		unsigned long interrupt, unsigned long reset)
{
	// 装载初始值
	WTDAT = wtdat;
//	WTCNT = wtdat;
	
	// 配置控制寄存器
	// Watchdog timer control register
	// [0], Reset enable/disable; 1:enable, 0:disable;
	// [2], Interrupt generation; 1:enable, 0:disable;
	// [4:3], Clock division factor; 00:16, 01:32, 10:64, 11:128;
	// [5], Watchdog timer; 1:enable, 0:disable;
	// [15:8], Prescaler value; 8-bit prescaler;
	WTCON = ((prescaler-1) << 8) | (divider << 3) | (interrupt << 2) | (reset << 0);
	watchdog_start();
} // }}}

// {{{ 开启看门狗定时器
void watchdog_start()
{
	WTCON |= (1 << 5);
} // }}} 

// {{{ 关闭看门狗定时器
void watchdog_stop()
{
	WTCON &= ~(1 << 5); //关闭看门狗定时器
} // }}}

// {{{ Watchdog Timer 中断服务函数
void watchdog_irq_handler(void)
{
	// 0. 现场保护, 保存通用寄存器数据
	__asm__("stmdb sp!, {r0-r12, lr}");
	// 1. 中断处理
	UART0_SendString("\r\nWatchdog interrupt!\r\n");
	// 2. 中断清零
	WTCLRINT = 1;
	// 中断执行地址清零
	// 注意与VIC0VECTADDR寄存器进行区分; 
	// 存储当前中断服务函数地址，中断结束后要清零。
	VIC0ADDRESS = 0;
	// 3. 现场恢复
	// ldmia汇编指令, 与stmdb指令对应, 从栈中按顺序将值读取回寄存器
	// '^'表示把spsr恢复到cpsr
	// 这里把之前lr的值赋给PC, 实现跳转;
	__asm__("ldmia sp!, {r0-r12, pc}^");
}//}}}
