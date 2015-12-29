/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < uart.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/09/27 >
    > Last Changed: 	 < 2015/09/28 >
    > Description:		FriendlyARM - Tiny6410 裸机程序 - UART
						UART 串口设置之输入输出字符

	适用UART0，连线：
	tinyADK(1312B) - COM0 -> 串口线 -> PC

****************************************************************/

#include "uart.h" 

// {{{ UDIVSLOT_LIST, S3C6410X.pdf - Pages 1101
static const int UDIVSLOT_LIST[16] = 
{
	0x0000, 0x0080, 0x0808, 0x0888,
	0x2222, 0x4924, 0x4A52, 0x54AA,
	0x5555, 0xD555, 0xD5D5, 0xDDD5,
	0xDDDD, 0xDFDD, 0xDFDF, 0xFFDF
};
//}}}

//{{{ UART0 通道引脚配置为UART功能 
void UART0_Port_Init()
{
	// RXD0 - XuRXD0/GPA0
	// TXD0 - XuTXD0/GPA1
	// GPACON[3:0] - GPA0 - 0010 B, UART RXD[0];
	// GPACON[7:4] - GPA1 - 0010 B, UART TXD[0];
	GPACON &= ~(GPA0_mask + GPA1_mask);
	GPACON |= (GPA0_UART + GPA1_UART);		// 0010 0010 B;
}	
//}}}

//{{{ UART0 - 配置波特率
void UART0_Set_BaudRate()
{
	// DIV_VAL = (PCLK / (bps x 16 ) ) - 1 = (66500000/(115200x16))-1 = 35.08
	// DIV_VAL = 35.08 = UBRDIVn + (num of 1’s in UDIVSLOTn)/16 
	UBRDIV0   = UART0_UBRDIV0;
	UDIVSLOT0 = UDIVSLOT_LIST[UART0_UDIVSLOT_NUM];
}
//}}}

//{{{ UART0 - 配置传输格式
void UART0_Set_Format()
{
	ULCON0 = (WORDLENGTH + STOPBITNUMBER + PARITYMODE + INFRAREDMODE); 
	UCON0 = (RECEIVEMODE + TRANSMITMODE + CLOCKSELECTION + RXERRORINTERRUPT);
	UFCON0 = (FIFOENABLE + RXFIFORESET + TXFIFORESET + RXFIFOTRIGGER + TXFIFOTRIGGER);
	UMCON0 = 0;						// 无流控

	/*
	ULCON0 = 0x3;  					// 数据位:8, 无校验, 停止位: 1, 8n1 
	UCON0  = 0x5;  					// 时钟：PCLK，禁止中断，使能UART发送、接收 
	UFCON0 = 0x01; 					// FIFO ENABLE
	UMCON0 = 0;						// 无流控
	*/
}
//}}}

//{{{ UART0 - 发送字符
void UART0_PutChar(char data)
{
	while (UFSTAT0 & (1<<14)); 		// 如果TX FIFO满，等待 
	// TX FIFO未满, 则跳出循环, 将输入写入UTXH0;
	UTXH0 = data;                      // 写数据 
}
//}}}

//{{{ UART0 - 接收字符
char UART0_GetChar()
{
	while ((UFSTAT0 & 0x7f) == 0);  // 如果RX FIFO空，等待 
	// RX FIFO接收到数据, 跳出循环;
	return URXH0;                   // 取数据 
}
//}}}

// {{{ UART0 - 发送字符串
void UART0_SendString(char *string)
{
	int index;

	for(index = 0; string[index] != '\0' && (index < MAX_STRING_LENGTH); index++)
	{
		while (UFSTAT0 & (1 << 14));
		UTXH0 = string[index];
	}
}

//}}}

// {{{ UART0 - 以十进制数字形式打印unsigned char型数据
void UART0_SendData8(unsigned char data)
{
	// unsigned char, 8bit, 0~255
	int hundreds = data / 100;
	int tens = data % 100 / 10;
	int digits = data % 10;

	UART0_PutChar(hundreds + '0');
	UART0_PutChar(tens + '0');
	UART0_PutChar(digits + '0');

}//}}}

//{{{ UART0 - 以十六进制数字形式打印unsigned lont int型数据
void UART0_SendData32(unsigned long int data)
{
	char hex_string[] = "00000000";
	unsigned char ch = 0;
	char index = 8;
	
	while(data)
	{
		// 获得最低4bit数据
		ch = (unsigned char)(data & 0x0000000F);
		
		switch(ch)
		{
			case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:
				hex_string[index-1] = '0'+ch;
				break;
			case 10:case 11:case 12:case 13:case 14:case 15:
				hex_string[index-1] = 'A'+ ch - 10;
				break;
			default:
				hex_string[index-1] = 'X';
				break;
		}	
		data = data >> 4;
		index--;
	}

	UART0_SendString("0x");
	UART0_SendString(&hex_string[index]);

} // }}}

// {{{ UART0 - 以十六进制数字形式打印unsigned lont int型数据(32 bit), 自动换行
void UART0_SendData32_n(unsigned long int data)
{
	UART0_SendData32(data);
	UART0_SendString("\n\r");
} // }}}
