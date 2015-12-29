/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < timer.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/10/29 >
  > Last Changed: 
  > Description:		S3C6410 - Timer - 裸机程序
 ****************************************************************/
#include "timer.h"
#include "leds.h"
#include "irq.h"

// {{{ 定时器中断初始化(VIC寄存器配置), 此函数应在irq_init()函数中调用;
void timer_irq_init(void)
{
	// VIC0 - No.23 - INT_TIMER0 - Timer 0 interrupt
	// VIC0 - No.24 - INT_TIMER1 - Timer 1 interrupt
	// VIC0 - No.25 - INT_TIMER2 - Timer 2 interrupt
	// VIC0 - No.26 - INT_WDT - Watchdog timer interrupt
	// VIC0 - No.27 - INT_TIMER3 - Timer 3 interrupt
	// VIC0 - No.28 - INT_TIMER4 - Timer 4 interrupt
#ifdef IRQ_TIMER_0
	// 设置中断类型
	VIC0INTSELECT &= ~(1 << 23); // 0, IRQ; 1, FIQ;

	// 设置ISR地址
	VIC0VECTADDR(23) = (unsigned int)timer0_irq_handler; 

	VIC0INTENABLE |= (1 << 23);	// 在中断控制器里使能timer0中断, INT_TIMER0
#endif

} // }}}

// {{{ 定时器初始化
// parameters: 
// 			  timer_id, 定时器编号;			0~4
//            prescaler, 预分频系数;		8 bit, 1~255
// 			  divider, 固定分频通道;		0: 1/1, 1: 1/2, 2: 1/4, 3: 1/8, 4: 1/16, 5: External TCLKn
//            tcntbn, 初始计数值; 			
//            tcmpbn, 比较值;
//
// Timer input clock Frequency = PCLK / ( {prescaler value + 1} ) / {divider value}
//
// timer_init(0, 66, 4, 62500, 0), prescaler = prescaler value + 1, divider value = 2^4 = 16; 
// 66MHZ / 66 / 16 = 62500HZ; TCNTB设置为62500, 则需要减62500次才能到0, 才能产生中断。即定时1s;
void timer_init(unsigned long timer_id, unsigned long prescaler, unsigned long divider, unsigned long tcntbn, unsigned long tcmpbn)
{
	unsigned long temp_data;

	// 设置预分频系数
	temp_data = TCFG0;
	// [7:0], Prescaler 0, Prescaler 0 value for timer 0 & 1;
	temp_data = (temp_data & (~(0xff))) | ((prescaler-1) << 0);
	TCFG0 = temp_data;

	// 设置固定分频通道
	temp_data = TCFG1;
	// [3:0], Divider MUX0, Select Mux input for PWM Timer 0;
	//        0000:1/1, 0001:1/2, 0010:1/4, 0011:1/8, 0100: 1/16, 0101~0111: External TCLK0
	// [7:4], Divider MUX1;
	// [11:8], Divider MUX2;
	// [15:12], Divider MUX3;
	// [19:16], Divider MUX4; 	
	temp_data = (temp_data & (~(0xf<< (4 * timer_id)))) | (divider << (4 * timer_id));
	// [23:20], Select DMA Request Channel Select Bit;
	// 0000: No select, 0001: INT0, 0010: INT1, 0011: INT2
	// 0100: INT3, 0101: INT4, 0110~0112: No select
	temp_data = (temp_data & (~(0xf<< 20))); // No select
	TCFG1 = temp_data;

	// TCNTBn, Timer n Count Buffer Register, 定时器初始值配置
	TCNTB0 = tcntbn;
	// TCMPBn, Timer 0 Compare Buffer Register, 定时器比较值配置 
	TCMPB0 = tcmpbn;

	// TCON, Timer Control Register, 定时器控制寄存器配置
	// [0], Timer 0 Start/Stop; 0: Stop , 1: Start Timer 0
	// [1], Timer 0 Manual Update; 0:NoOperation , 1:UpdateTCNTB0,TCMPB0
	// [2], Timer 0 Output Inverter on/off; 0: Inverter Off, 1: TOUT0 Inverter-On;
	// [3], Timer 0 Auto Reload on/off; 0: One-Shot, 1: Interval Mode(Auto-Reload);
	// [4], Dead zone enable/disable; 0:disable, 1:enable;
	TCON |= 1<<1;	// Update TCNTB0, TCMPB0
	TCON &= ~(1<<1);	// 清手动更新位
	TCON |= (1<<0) | (1<<3);// 启动timer0, 并设置为自动加载模式;

	// TINT_CSTAT, Timer Interrupt Control and Status Register, 定时器中断配置
	// [0], Timer 0 interrupt Enable; 1 - Enabled , 0 - Disabled
	// [1], Timer 1 interrupt Enable;
	// [2], Timer 2 interrupt Enable;
	// [3], Timer 3 interrupt Enable;
	// [4], Timer 4 interrupt Enable;
	//
	// [5], Timer 0 Interrupt Status; Timer 0 Interrupt Status Bit. Clears by writing '1' on this bit.
	// [6], Timer 1 Interrupt Status;
	// [7], Timer 2 Interrupt Status;
	// [8], Timer 3 Interrupt Status;
	// [9], Timer 4 Interrupt Status;
	temp_data = TINT_CSTAT;
	temp_data |= (1 << (timer_id));
	TINT_CSTAT = temp_data;
}//}}}

// {{{ Timer0中断服务函数 - VIC0 - No.23 - INT_TIMER0 - Timer 0 interrupt
void timer0_irq_handler(void)
{
	static unsigned int my_led_flag = 0;
	// 0. 现场保护, 保存通用寄存器数据
	__asm__("stmdb sp!, {r0-r12, lr}");
	// 1. 中断处理
	my_led_flag = (my_led_flag + 1) % 2;
	if(my_led_flag == 1)
		leds_ON(1);
	else
		leds_OFF(1);

	// 2. 中断清零
	// Timer 0 中断状态清零 
	// [5], Timer 0 Interrupt Status Bit. Clears by writing '1' on this bit.
	TINT_CSTAT |= (1 << 5); 
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
