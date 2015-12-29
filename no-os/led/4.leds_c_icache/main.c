/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/07/30 >
    > Last Changed: 
    > Description:		C语言实现流水灯
						FriendlyARM - Tiny6410 裸机程序 

 	功能：LED点灯程序，点亮LED1,LED2,LED3,LED4
	LED对应的GPIO,要查看对应电路图。
	对应GPIO的控制寄存器和数据寄存器,要查看相应的芯片手册

	LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
	LED2 -> GPK5 ->	GPKCON0[23:20] / GPKDAT[5]
	LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
	LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
	低电平点亮LED灯
****************************************************************/

// 延时函数定义;
void delay()
{
	volatile int i = 0x10000;
	while (i--);
}

int main()
{
	int i = 0;

	// 配置LED灯对应的GPIO引脚
	volatile unsigned long *gpkcon0 = (volatile unsigned long *)0x7F008800;
	volatile unsigned long *gpkdat = (volatile unsigned long *)0x7F008808;
	// Volatile是一个变量声明限定词。它告诉编译器，它所修饰的变量的值可能会在任何时刻被意外的更新 
	/**************************
	当一个变量的内容可能会被意想不到的更新时，一定要使用volatile来声明该变量。
	通常，只有三种类型的变量会发生这种"意外"：
	1. 在内存中进行地址映射的设备寄存器；(如本程序)
	2. 在中断处理程序中可能被修改的全局变量；
	3. 多线程应用程序中的全局变量；
	参考:http://bbs.21ic.com/icview-125510-1-1.html
	************************/
	
	//0001 0001 0001 0001 0000 0000 0000 0000 b;															  
	//GPK4, 位[19:16], 值为0001, 配置为Output;
	//GPK5, bit[23:20], 0001; 同理, GPK6,GPK7均为Output;
	*gpkcon0 = 0x11110000;

	// 跑马灯(观测灭的LED灯)
	// i的初始值为0x10, 0001 0000b; 即只有LED1灭;
	i = 0x10; 
	while (1)
	{
		*gpkdat = i;
		i++;
		if (i == 0x100 ) // 加到边界，则返回; 保证LED灯不断变化;
			i = 0x10;
		delay();
	}
	
	return 0;
}
