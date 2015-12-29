/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < irq.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/26 >
    > Last Changed: 
    > Description:		S3C6410 - Vectored Interrupt Controller ( PL192 )
****************************************************************/


#ifndef __irq__
#define __irq__

// {{{ 中断寄存器配置
// EINT0CON0, External Interrupt 0(Group0) Configuration Register 0
// 000 = Low level, 001 = High level, 01x = Falling edge triggered, 10x = Rising edge triggered, 11x = Both edge triggered
// EINT1,0 [2:0] 
// EINT3,2 [6:4]
// EINT5,4 [10:8]
// ....
#define EINT0CON0  			(*((volatile unsigned long *)0x7F008900))
// EINT0MASK, External Interrupt 0(Group0) Mask Register
// 0 = Enable Interrupt, 1= Masked
// EINT0, [0] 
// EINT1, [1] 
// .....
#define EINT0MASK  			(*((volatile unsigned long *)0x7F008920))
// External Interrupt 0(Group0) Pending Registe
// 0 = Not occur, 1= Occur interrupt 
#define EINT0PEND  			(*((volatile unsigned long *)0x7F008924))
// EINT0FLTCON0, External Interrupt 0(Group0) Filter Control Register 0
// EINT0, 1, [5:0], Filtering width of EINT0,1; This value is valid when FLTSEL is 1.
// FLTSEL, [6], Filter Selection for EINT0,1: 0 = delay filter, 1 = digital filter(clock count);
// FLTEN, [7], Filter Enable for EINT 0,1: 0 = disables, 1 = enabled;
#define EINT0FLTCON0		(*((volatile unsigned long *)0x7F008910))
#define PRIORITY 	    	(*((volatile unsigned long *)0x7F008280))
#define SERVICE     		(*((volatile unsigned long *)0x7F008284))
#define SERVICEPEND 		(*((volatile unsigned long *)0x7F008288))
// IRQ Status Register (VIC0)
// Show the status of the interrupts after masking by the VICxINTENABLE and VICxINTSELECT Registers
// 0 = interrupt is inactive (reset), 1 = interrupt is active. 
#define VIC0IRQSTATUS  		(*((volatile unsigned long *)0x71200000))
#define VIC0FIQSTATUS  		(*((volatile unsigned long *)0x71200004))
// Raw Interrupt Status Register (VIC0)
// Show the status of the FIQ interrupts before masking by the VICINTENABLE and VICINTSELECT Registers
// 0 = interrupt is inactive before masking, 1 = interrupt is active before masking;
#define VIC0RAWINTR    		(*((volatile unsigned long *)0x71200008))
// Interrupt Select Register (VIC0) 
// 0 = IRQ interrupt (reset), 1 = FIQ interrupt
#define VIC0INTSELECT  		(*((volatile unsigned long *)0x7120000c))
// Interrupt Enable Register (VIC0)
// 0 = interrupt disabled (reset), 1 = Interrupt enabled
#define VIC0INTENABLE  		(*((volatile unsigned long *)0x71200010))
// Interrupt Enable Clear Register (VIC0)
// 0 = no effect, 1 = interrupt disabled in VICINTENABLE Register.
#define VIC0INTENCLEAR 		(*((volatile unsigned long *)0x71200014))
#define VIC0PROTECTION 		(*((volatile unsigned long *)0x71200020))
#define VIC0SWPRIORITYMASK 	(*((volatile unsigned long *)0x71200024))
#define VIC0PRIORITYDAISY  	(*((volatile unsigned long *)0x71200028))

//Interrupt Enable Clear Register (VIC0)
// 0 = no effect, 1 = interrupt disabled in VICINTENABLE Register.
#define VIC0INTENCLEAR		(*((volatile unsigned long *)0x71200014))

// Vector Address Register (VIC0)
// Contains the address of the currently active ISR, with reset value 0x00000000.
// 注意与VIC0VECTADDR寄存器进行区分; 
// 存储当前中断服务函数地址，中断结束后要清零。
#define VIC0ADDRESS        	(*((volatile unsigned long *)0x71200f00))
#define VIC1ADDRESS        	(*((volatile unsigned long *)0x71300f00))

// 中断服务函数地址
// Vector Address [31:0] Register (VIC0), 0x7120_0100 ~ 0x7120_017C
#define VIC0VECTADDR(x)	   	(*((volatile unsigned long *)(0x71200100 + (x*4))))
// Vector Address [31:0] Register (VIC1), 0x7130_0100 ~ 0x7130_017C
#define VIC1VECTADDR(x)	   	(*((volatile unsigned long *)(0x71300100 + (x*4))))
//}}}

// 中断初始化
void irq_init(void);

#endif
