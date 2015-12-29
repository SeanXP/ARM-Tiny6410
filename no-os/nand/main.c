/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/09/27 >
    > Last Changed:      < 2015/10/21 >
    > Description:		Frindly ARM - Tiny6410 裸机程序
						UART 串口通信
****************************************************************/

#include "uart.h"
#include "nand.h"

int main()
{
	char data;

	UART0_Port_Init();
	UART0_Set_BaudRate();
	UART0_Set_Format();
	
	UART0_SendString("\n\r-----------------------------------------\n\r");

	// 打印Nand Flash ID
	// {{{
	{
		int i = 0;
		char id[6] = {0,0,0,0,0,0};
		nand_readID(id);
		UART0_SendString("Read Nand Flash ID:\n\r");
		for(i = 0; i < 6; i++)
		{
			UART0_SendData32(id[i]);
			UART0_SendString("\t");
		}
		UART0_SendString("\n\r");
	} // }}}

	// Nand Flash Read & Write {{{
	{
		// Read a page from start_addr, read (size) bytes to *buffer;
		// 第2048块的第0页:
		// Address = 块号x块大小 + 页号x页大小 + 页内地址
		//		   = 2048 x 128k + 0 x 2k + 0 = 0x1000_0000;
		// ----------------------------------------------------------------------------------------
		// Block Address(A29~A17) : 0b0001_0000_0000_000x_xxxx_xxxx_xxxx_xxxx, 第2048块/ 总共8192块
		// Page Address(A16~A11):	0bxxxx_xxxx_xxxx_xxx0_0000_0xxx_xxxx_xxxx, 第0页 / 64页
		// Column Address(A10~A0):	0bxxxx_xxxx_xxxx_xxxx_xxxx_x000_0000_0000, 第0字节 / 2048字节
		// ----------------------------------------------------------------------------------------
		// Block Address(A29~A17) : 0b0011_1111_1111_111x_xxxx_xxxx_xxxx_xxxx, 第8191块/ 总共8192块
		// Page Address(A16~A11):	0bxxxx_xxxx_xxxx_xxx1_1111_1xxx_xxxx_xxxx, 第63页 / 64页
		// Column Address(A10~A0):	0bxxxx_xxxx_xxxx_xxxx_xxxx_x111_1111_1111, 第2047字节/ 2048字节
		// ----------------------------------------------------------------------------------------
		char buffer[4096];
		int i = 0;

		//第0块的第0,1页, 连读两页
		nand_read_page(buffer, 0x00000000, 4096); 

		UART0_SendString("\n\rBlock 0 - Page 0:\n\r");
		for(i = 2041; i < 2048; i++)
		{
			UART0_SendData32(buffer[i]);
			UART0_SendString("\t");
		}
		UART0_SendString("\n\rBlock 0 - Page 1:\n\r");
		for(i = 2048; i < 2058; i++)
		{
			UART0_SendData32(buffer[i]);
			UART0_SendString("\t");
		}

		//第0块的第1页
		nand_read_page(buffer, 0x00000800, 2048); 
		UART0_SendString("\n\rBlock 0 - Page 1:\n\r");
		for(i = 0; i < 10; i++)
		{
			UART0_SendData32(buffer[i]);
			UART0_SendString("\t");
		}
		UART0_SendString("\n\r");

		// 找一块没有使用的Nand Flash页面, 内容都是空, 即0xFF;
		//第2048块的第1页
		nand_read_page(buffer, 0x10000000, 2048); 
		UART0_SendString("\n\rBlock 2048 - Page 0:\n\r");
		for(i = 0; i < 10; i++)
		{
			UART0_SendData32(buffer[i]);
			UART0_SendString("\t");
		}
		UART0_SendString("\n\r");

		//修改值，再写入Nand Flash;
		for(i = 0; i < 5; i++)
			buffer[i] = i;
		nand_write_page(buffer, 2048, 0x10000000);

		// 重新读
		nand_read_page(buffer, 0x10000000, 2048); 
		UART0_SendString("\n\rBlock 2048 - Page 0:\n\r");
		for(i = 0; i < 10; i++)
		{
			UART0_SendData32(buffer[i]);
			UART0_SendString("\t");
		}
		UART0_SendString("\n\r");
	}//}}}

	while(1)
	{
		data = UART0_GetChar();
		UART0_PutChar(data+1);
	}

    return 0;
}
