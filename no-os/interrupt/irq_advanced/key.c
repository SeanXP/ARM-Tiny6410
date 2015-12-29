/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < key.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/08/12 >
    > Last Changed: 
	> Description:		C语言实现按键控制, 按键API设计
  						FriendlyARM - Tiny6410 裸机程序 

  Button对应的GPIO, 在Tiny6140底层扩展板(TinyADK 1312B版本) / 核心板(Tiny6410 1308):
  K1 -> XEINT16 -> 核心板P1排针-A53引脚 -> 核心板XEINT0 -> GPN0 -> GPNCON[1:0] / GPNDAT[0] 
  K2 -> XEINT17 -> 核心板P1排针-A54引脚 -> 核心板XEINT0 -> GPN1 -> GPNCON[3:2] / GPNDAT[1] 
  K3 -> XEINT18 -> 核心板P1排针-A55引脚 -> 核心板XEINT0 -> GPN2 -> GPNCON[5:4] / GPNDAT[2]
  K4 -> XEINT19 -> 核心板P1排针-A56引脚 -> 核心板XEINT0 -> GPN3 -> GPNCON[7:6] / GPNDAT[3]

  按键硬件连接上拉电阻，被按下, 对应IO被拉为低电平;

****************************************************************/

#include "key.h"
#include "common.h"	// #define BIT0~BIT15
#include "uart.h"	// UART0_SendString();
#include "leds.h"	// leds_ON();
#include "irq.h"	// VIC0ADDRESS

// {{{ 配置key的相关GPIO端口为输入;
void key_init(void)
{
	GPNCON &= ~(GPN0_mask + GPN1_mask + GPN2_mask + GPN3_mask);
	GPNCON |=  (GPN0_in + GPN1_in + GPN2_in + GPN3_in);
} //}}}

// {{{ 配置key的相关GPIO端口为中断引脚;
	// 配置GPN0~3引脚为中断功能(KEY 1~4)
	// Key1, GPN0, External interrupt Group 0, EINT0
	// Key2, GPN1, External interrupt Group 0, EINT1
	// Key3, GPN2, External interrupt Group 0, EINT2
	// Key4, GPN3, External interrupt Group 0, EINT3
void key_irq_init(void)
{
	GPNCON &= ~(GPN0_mask + GPN1_mask + GPN2_mask + GPN3_mask);
	GPNCON |=  (GPN0_int + GPN1_int + GPN2_int + GPN3_int);

	// 设置中断触发方式为: 下降沿触发(0b01x)
	// EINT1,0 [2:0], EINT3,2 [6,4]
	EINT0CON0 &= ~(0xff);
	EINT0CON0 |= 0x33;		// 0b0011_0011

	// 硬件滤波配置
	// EINT0FLTCON0, External Interrupt 0(Group0) Filter Control Register 0
	// EINT0, 1, [5:0], Filtering width of EINT0,1; This value is valid when FLTSEL is 1.
	// FLTSEL, [6], Filter Selection for EINT0,1: 0 = delay filter, 1 = digital filter(clock count);
	// FLTEN, [7], Filter Enable for EINT 0,1: 0 = disables, 1 = enabled;
	// [8:13], [14], [15], EINT 2,3;
//	EINT0FLTCON0 |= 0x1010;
		// 0b 1000_0000_1000_0000
	EINT0FLTCON0 |= 0xcfff;
		// 0b 1100_1111_1111_1111

	// 禁止屏蔽中断, 1:屏蔽中断, 0:不屏蔽
	EINT0MASK &= ~(0xf);	// 0b0000
	// 设置中断类型
	VIC0INTSELECT &= ~(0x1); // 0, IRQ; 1, FIQ;

	// 设置ISR地址
	VIC0VECTADDR(0) = (unsigned int)key_irq_handler; 

	// 在中断控制器里使能这些中断
	// [1], INT_EINT0 : External interrupt Group 0 (EINT0~EINT3)
	VIC0INTENABLE |= (0x1); 
} //}}}

//{{{ 返回按键的值;
char key_1()
{
	return (BUTTON1);
}
char key_2()
{
	return (BUTTON2);
}
char key_3()
{
	return (BUTTON3);
}
char key_4()
{
	return (BUTTON4);
}//}}}

// {{{ Key1~4外部中断服务函数 - VIC0 - INT_EINT0 - External interrupt Group 0: 0 ~ 3
void key_irq_handler(void)
{
	// 0. 现场保护, 保存通用寄存器数据
	__asm__("stmdb sp!, {r0-r12, lr}");
	// 1. 中断处理
	
	// Key interrupt 共有4个中断, EINT0 ~ EINT3
	// 通过EINT0PEND进行判断
	if(EINT0PEND & 0x1)	// EINT0
	{
		leds_ON(1);
		UART0_SendString("C irq - key 1\n\r");
	}
	if(EINT0PEND & 0x2) // EINT1
	{
		leds_ON(2);
		UART0_SendString("C irq - key 2\n\r");
	}
	if(EINT0PEND & 0x4)	// EINT2
	{
		leds_ON(3);
		UART0_SendString("C irq - key 3\n\r");
	}
	if(EINT0PEND & 0x8) // EINT3
	{
		leds_OFF_all();
		UART0_SendString("C irq - key 4\n\r");
	}

	// 2. 中断清零
	// 中断挂起标志位清零, Each bit is cleared by writing "1";
	EINT0PEND = (0xf);    // EINT0~3
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
