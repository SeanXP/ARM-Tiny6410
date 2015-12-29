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

// 波特率设置
#define UART0_CLK			66500000	// PCLK, 66.5 MHZ
#define UART0_BAUDRATE		115200		// Baud Rate	
//#define UART0_BAUDRATE		9600		// Baud Rate	
//#define UART0_BAUDRATE		38400		// Baud Rate	
// 传输格式配置
#define WORD_LENGTH			8			// the number of data bits, WORD_LENGTH = [5,8];	
#define STOPBIT_NUMBER		0			// stop bits; 0, (1 stop bit); 1, (2 stop bits); 
#define PARITY_MODE			0			// 0, No parity; 4, Odd parity; 5, Even parity;
#define INFRA_RED_MODE		0			// 0, Normal mode; 1, Infra-Red Tx/Rx mode;
// Receive/Transmit Mode, 00 = Disable, 01 = Interrupt request or polling mode, 10 = DMA request (DMA_UART0), 11 = DMA request (DMA_UART1)
#define RECEIVE_MODE		1
#define TRANSMIT_MODE		1		
#define CLOCK_SELECTION		0		// PCLK 
#define RX_ERROR_INTERRUPT	0		// 0, disable; 1, enable;
#define FIFO_ENABLE			1		// 0, disable; 1, enable;
#define RX_FIFO_RESET		0		// 0, normal; 1, Rx FIFO reset;
#define TX_FIFO_RESET		0		// 0, normal; 1, Tx FIFO reset;
#define RX_FIFO_TRIGGER		0		
#define TX_FIFO_TRIGGER		0		

#define MAX_STRING_LENGTH	250		//发送字符串的最大长度

// {{{ UDIVSLOT_LIST, S3C6410X.pdf - Pages 1101
const int UDIVSLOT_LIST[16] = 
{
	0x0000, 0x0080, 0x0808, 0x0888,
	0x2222, 0x4924, 0x4A52, 0x54AA,
	0x5555, 0xD555, 0xD5D5, 0xDDD5,
	0xDDDD, 0xDFDD, 0xDFDF, 0xFFDF
};
//}}}

//{{{ UART引脚配置
#define GPACON     (*((volatile unsigned long *)0x7F008000))
#define GPA0_UART		(2<<(0*4))	// 0010 = UART
#define GPA1_UART		(2<<(1*4))
//GPACON的位掩码
#define GPA0_mask		(0xf<<(0*4))
#define GPA1_mask		(0xf<<(1*4))
// }}}

//{{{ UART0 寄存器宏定义

// UART LINE CONTROL REGISTER - ULCON0 (reset value -> 0x0)
// [1:0], Word Length, 00 = 5-bit 01 = 6-bit 10 = 7-bit 11 = 8-bit;
// [2], Number of Stop Bit, 0 = One stop bit per frame 1 = Two stop bit per frame
// [5:3], Parity Mode, 100 = Odd parity,101 = Even parity	
// [6], Infra-Red Mode, 0 = Normal mode operation 1 = Infra-Red Tx/Rx mode
#define ULCON0     (*((volatile unsigned long *)0x7F005000))
#define WORDLENGTH			((WORD_LENGTH - 5) 	<< 0)
#define STOPBITNUMBER		((STOPBIT_NUMBER) 	<< 2)
#define PARITYMODE			((PARITY_MODE) 		<< 3)
#define INFRAREDMODE		((INFRA_RED_MODE) 	<< 6)

// UART CONTROL REGISTER - UCON0 (reset value -> 0x00)
// [1:0], Receive Mode 
// [3:2], Transmit Mode 
// Receive/Transmit Mode, 00 = Disable, 01 = Interrupt request or polling mode, 10 = DMA request (DMA_UART0), 11 = DMA request (DMA_UART1)
// [4], Send Break Signal, 0 = Normal transmit 1 = Send break signal;
// [5], Loop-back Mode, 0 = Normal operation 1 = Loop-back mode;
// [6], Rx Error Status Interrupt Enable, 0 = not generate, 1 = Generate receive error status interrupt;
// [7], Rx Time Out Enable, 0 = Disable 1 = Enable;
// [8], Rx Interrupt Type, 0 = Pulse, 1 = Level;
// [9], Tx Interrupt Type, 0 = Pulse, 1 = Level;
// [11:10], Clock Selection, x0=PCLK, 01 = EXT_UCLK0, 11 = EXT_UCLK1;
#define UCON0      (*((volatile unsigned long *)0x7F005004))
#define RECEIVEMODE			((RECEIVE_MODE) 		<< 0)
#define TRANSMITMODE		((TRANSMIT_MODE) 		<< 2)	
#define RXERRORINTERRUPT	((RX_ERROR_INTERRUPT) 	<< 6)
#define CLOCKSELECTION		((CLOCK_SELECTION) 		<< 10)

// UART FIFO CONTROL REGISTER, UFCON0 (reset value -> 0x0)
// [0], FIFO Enable, 0 = Disable 1 = Enable;
// [1], Rx FIFO Reset, 0 = Normal 1= Rx FIFO reset;
// [2], Tx FIFO Reset, 0 = Normal 1= Tx FIFO reset;
// [3], Reserved;
// [5:4], Rx FIFO Trigger Level, 00 = 1-byte 01 = 8-byte, 10 = 16-byte 11 = 32-byte;
// [7:6], Tx FIFO Trigger Level, 00 = Empty 01 = 16-byte, 10 = 32-byte 11 = 48-byte;
#define UFCON0     (*((volatile unsigned long *)0x7F005008))
#define FIFOENABLE		((FIFO_ENABLE) 		<< 0)
#define RXFIFORESET		((RX_FIFO_RESET) 	<< 1)
#define TXFIFORESET		((TX_FIFO_RESET) 	<< 2)
#define RXFIFOTRIGGER	((RX_FIFO_TRIGGER) 	<< 4)
#define TXFIFOTRIGGER	((TX_FIFO_TRIGGER) 	<< 6)


#define UMCON0     (*((volatile unsigned long *)0x7F00500C))
#define UTRSTAT0   (*((volatile unsigned long *)0x7F005010))
#define UFSTAT0    (*((volatile unsigned long *)0x7F005018))
#define UTXH0      (*((volatile unsigned char *)0x7F005020))
#define URXH0      (*((volatile unsigned char *)0x7F005024))
#define UBRDIV0    (*((volatile unsigned short *)0x7F005028))
#define UDIVSLOT0  (*((volatile unsigned short *)0x7F00502C))

#define UART0_DIV_VAL		( (UART0_CLK) / ((UART0_BAUDRATE) * 16) - 1)
#define UART0_UBRDIV0		(int)(UART0_DIV_VAL)
#define UART0_UDIVSLOT_NUM	(int)((UART0_DIV_VAL - UART0_UBRDIV0) * 16)
// }}}

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
