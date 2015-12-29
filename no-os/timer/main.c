/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/09/27 >
    > Last Changed: 
    > Description:		Frindly ARM - Tiny6410 裸机程序
						UART 串口通信
****************************************************************/

#include "uart.h"
#include "leds.h"
#include "timer.h"
#include "irq.h"

//{{{
void print_cpsr(unsigned int cpsr, char *why)
{
	//printf("\n\rreason=%s cpsr = 0x%x\n\r", why, cpsr);
	UART0_SendString("\n\rInterrupt Reason:");
	UART0_SendString(why);
	UART0_SendString(",\tCPSR = ");
	UART0_SendData32(cpsr);
	UART0_SendString("\n\r");


	UART0_SendString("\t[31], N, Negative:");
	UART0_SendData32((cpsr >> 31) & 0x1);

	UART0_SendString("\t[30], Z, Zero:");
	UART0_SendData32((cpsr >> 30) & 0x1);

	UART0_SendString("\t[29], C, Carry:");
	UART0_SendData32((cpsr >> 29) & 0x1);

	UART0_SendString("\t[28], V, Overflow:");
	UART0_SendData32((cpsr >> 28) & 0x1);

	UART0_SendString("\n\r\t[7], I, IRQ:");
	UART0_SendData32((cpsr >> 7) & 0x1);

	UART0_SendString("\t[6], F, FIQ:");
	UART0_SendData32((cpsr >> 6) & 0x1);

	UART0_SendString("\t[5], T, Thumb:");
	UART0_SendData32((cpsr >> 5) & 0x1);

	UART0_SendString("\t[4:0], Mode:");
	UART0_SendData32((cpsr) & 0x1f);

	UART0_SendString("\n\r");
}

//}}}

int main()
{
	char data;
	
	leds_init();
	//开始默认关闭LED灯
	leds_OFF_all();

	timer_init(0, 66, 4, 65000, 0);

	// 串口初始化, 在start.S中已经调用过
	UART0_Port_Init();
	UART0_Set_BaudRate();
	UART0_Set_Format();

	
	// '\n' 换行
	// '\r' 回车
	UART0_SendString("\n\rHello World!\n\r");
	//UART0_SendString("Hello World!\n");

	while(1)
	{
		data = UART0_GetChar();
		irq_disable();
		UART0_PutChar(data);
		irq_enable();
	}
    return 0;
}
