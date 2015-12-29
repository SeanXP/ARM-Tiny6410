/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < timer.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/29 >
    > Last Changed: 
    > Description:		S3C6410 - Timer - 裸机程序
****************************************************************/


#ifndef __timer__
#define __timer__

#define IRQ_TIMER		// 开启定时器IRQ中断
#define IRQ_TIMER_0		// 开启Timer0 IRQ中断

// {{{ Timer Register Define
#define		PWMTIMER_BASE			(0x7F006000)

// Timer Configuration Register 0 that configures the two 8-bit Prescaler and DeadZone Length
// [7:0], Prescaler 0, Prescaler 0 value for timer 0 & 1;
// [15:8], Prescaler 1, Prescaler 1 value for Timer 2, 3 and 4;
// [23:16], Dead zone length;
#define		TCFG0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x00)) )

// Timer Configuration Register 1 that controls 5 MUX and DMA Mode Select Bit
// [3:0], Divider MUX0, Select Mux input for PWM Timer 0;
//		  0000:1/1, 0001:1/2, 0010:1/4, 0011:1/8, 0100: 1/16, 0101~0111: External TCLK0
// [7:4], Divider MUX1, Select Mux input for PWM Timer 1;
// [11:8], Divider MUX2;
//		  0000:1/1, 0001:1/2, 0010:1/4, 0011:1/8, 0100: 1/16, 0101~0111: External TCLK1
// [15:12], Divider MUX3;
// [19:16], Divider MUX4;
// [23:20], Select DMA Request Channel Select Bit; 
//			0000: No select, 0001: INT0, 0010: INT1, 0011: INT2, 0100: INT3, 0101: INT4, 0110~0112: No select
#define		TCFG1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x04)) )
// Timer Control Register
// [0], Timer 0 Start/Stop; 0: Stop , 1: Start Timer 0
// [1], Timer 0 Manual Update; 0:NoOperation , 1:UpdateTCNTB0,TCMPB0
// [2], Timer 0 Output Inverter on/off; 0: Inverter Off, 1: TOUT0 Inverter-On;
// [3], Timer 0 Auto Reload on/off; 0: One-Shot, 1: Interval Mode(Auto-Reload);
// [4], Dead zone enable/disable; 0:disable, 1:enable;
// [7:5], Reserved Bits;
// [8], Timer 1 Start/Stop; [9], Timer 1 Manual Update; [10], Timer 1 Output Inverter on/off; [11], Timer 1 Auto Reload on/off;
#define		TCON      	( *((volatile unsigned long *)(PWMTIMER_BASE+0x08)) )
// Timer 0 Count Buffer Register
#define		TCNTB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x0C)) )
// Timer 0 Compare Buffer Register
#define		TCMPB0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x10)) )
// Timer 0 Count Observation Register
#define		TCNTO0    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x14)) )
#define		TCNTB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x18)) )
#define		TCMPB1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x1C)) )
#define		TCNTO1    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x20)) )
#define		TCNTB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x24)) )
#define		TCMPB2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x28)) )
#define		TCNTO2    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x2C)) )
#define		TCNTB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x30)) )
#define		TCMPB3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x34)) )
#define		TCNTO3    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x38)) )
#define		TCNTB4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x3C)) )
#define		TCNTO4    	( *((volatile unsigned long *)(PWMTIMER_BASE+0x40)) )
// Timer Interrupt Control and Status Register
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
#define		TINT_CSTAT 	( *((volatile unsigned long *)(PWMTIMER_BASE+0x44)) )
//}}}

// 定时器初始化
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
void timer_init(unsigned long timer_id, unsigned long prescaler, unsigned long divider, 
		unsigned long tcntbn, unsigned long tcmpbn);

// 定时器中断初始化(VIC寄存器配置), 此函数应在irq_init()函数中调用;
void timer_irq_init(void);

// Timer0中断服务函数 - VIC0 - No.23 - INT_TIMER0 - Timer 0 interrupt
void timer0_irq_handler(void);

#endif
