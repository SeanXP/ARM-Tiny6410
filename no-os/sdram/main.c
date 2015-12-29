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

int main()
{
	char data;

	UART0_Port_Init();
	UART0_Set_BaudRate();
	UART0_Set_Format();
	
	// '\n' 换行
	// '\r' 回车
	UART0_SendString("Hello World!\n\r");
	//UART0_SendString("Hello World!\n");

	UART0_SendString("Send Data 8:");
	UART0_SendString("\n\r");
	UART0_SendData8(123);
	UART0_SendString("\n\r");
	UART0_SendData8(0xFF);
	UART0_SendString("\n\r");

	UART0_SendString("Send Data 32:");
	UART0_SendString("\n\r");
	UART0_SendData32(0x1234);
	UART0_SendString("\n\r");
	UART0_SendData32(0x1234ABCD);
	UART0_SendString("\n\r");
	UART0_SendData32(0xABCD);
	UART0_SendString("\n\r");
	UART0_SendData32(0x1234ABCD);
	UART0_SendString("\n\r");
	UART0_SendData32(0x1);
	UART0_SendString("\n\r");

	while(1)
	{
		data = UART0_GetChar();
		UART0_PutChar(data+1);
	}

    return 0;
}
