/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/21 >
    > Last Changed: 
    > Description:		Frindly ARM - Tiny6410 裸机程序
    					自定义stdio库 - printf / scanf
****************************************************************/
#include "stdio.h"
#include "uart.h"

int main()
{
	int a, b;
	
	UART0_Port_Init();
	UART0_Set_BaudRate();
	UART0_Set_Format();
	
	UART0_SendString("\n\r-----------------------------------------\n\r");

	printf("hello, world\n\r");

	while (1)
	{
		printf("please enter two number: \n\r");
		scanf("%d %d", &a, &b);
		printf("\n\r");
		printf("the sum is: %d\n\r", a+b);
	}
	
	return 0;
}
