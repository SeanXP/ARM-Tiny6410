/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < irq.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/26 >
    > Last Changed:		 < 2015/10/28 > 
    > Description:		S3C6410 - Vectored Interrupt Controller ( PL192 )
 ****************************************************************/

#include "irq.h"
#include "key.h"		// key_irq_init()
#include "timer.h"		// timer_irq_init()

// {{{ 中断初始化 void irq_init(void)
void irq_init(void)
{
	// 将协处理器P15的寄存器C1中的bit24置1，该比特位为VIC使能位
	__asm__(
			"mrc p15, 0, r0, c1, c0, 0\n"
			"orr r0, r0, #(1 << 24)\n"
			"mcr p15, 0, r0, c1, c0, 0"
		   );
	// 关闭所有中断
	VIC0INTENCLEAR = 0xffffffff;
	// 清空VICxADDR
	VIC0ADDRESS = 0;
	// 中断挂起标志位清零, Each bit is cleared by writing "1";
	EINT0PEND = 0xffffffff;

#ifdef IRQ_KEY		// 开启外部按键中断
	key_irq_init();
#endif

#ifdef IRQ_TIMER	// 开启定时器中断
	timer_irq_init();
#endif
}
//}}}

//{{{ IRQ中断禁止(CPSR [7]) void irq_disable(void)
void irq_disable(void)
{
	__asm__(
			"mrs r0, cpsr\n"
			"orr r0, r0, #(1 << 7)\n"	// 写1禁止
			"msr cpsr_c, r0\n"
		   );
} //}}}

//{{{ IRQ中断打开(CPSR [7]) void irq_enable(void)
void irq_enable(void)
{
	__asm__(
			"mrs r0, cpsr\n"
			"bic r0, r0, #(1 << 7)\n"	// 写0取消禁止
			"msr cpsr_c, r0\n"
		   );
}//}}}
