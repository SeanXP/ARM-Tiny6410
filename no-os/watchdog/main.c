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
#include "watchdog.h"

int main()
{
	char data;
	
	// 串口初始化, 在start.S中已经调用过
	UART0_Port_Init();
	UART0_Set_BaudRate();
	UART0_Set_Format();

	
	// 测试看门狗的定时功能
 	//watchdog_init(66, 0, 62500, 1, 0);	//仅中断, 不复位
 	//watchdog_init(66, 0, 62500, 1, 1);	//中断&复位, 配置复位后, 中断函数就鸡肋了, 不会被调用;

	// '\n' 换行
	// '\r' 回车
	UART0_SendString("\n\rHello World!\n\r");
	//UART0_SendString("Hello World!\n");

	while(1)
	{
		data = UART0_GetChar();
		UART0_PutChar(data);
	}
    return 0;
}
