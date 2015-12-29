/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < start.S >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/07/30 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 裸机程序 - 点亮LED灯

 	功能：LED点灯程序，点亮LED1,LED2,LED3,LED4
	LED对应的GPIO,要查看对应电路图。
	对应GPIO的控制寄存器和数据寄存器,要查看相应的芯片手册

	LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
	LED2 -> GPK5 ->	GPKCON0[23:20] / GPKDAT[5]
	LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
	LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
                        IO方式配置 /  IO数据配置
****************************************************************/
.global _start						

_start:
	// ----1. 配置ARM CPU寄存器的外设地址----
	// 把外设的基地址告诉CPU
    ldr r0, =0x70000000 					//对于6410来说,内存(0x00000000～0x60000000),外设(0x70000000-0x7fffffff)
	// 0x13 = 0001 0011 b
    orr r0, r0, #0x13						//外设大小:256M
	// 在基于ARM的嵌入式应用系统中，存储系统通常是通过系统控制协处理器CP15完成的。
	// CP15包含16个32位的寄存器，其编号为0~15。
	// MCR : ARM寄存器到协处理器寄存器的数据传送
    mcr p15,0,r0,c15,c2,4       			//把r0的值(包括了外设基地址+外设大小)告诉cpu
    
	// ----2. 关看门狗----
	//	Watch Dog - WDCON寄存器 - 0x7E004000 , Watchdog timer control register
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0] 							// 将r0寄存器的值写入地址值为r1的（存储器）内存中

	// ----3. 配置GPIO-GPK相关寄存器----
	// GPKCON0-0x7F008800,Port K Configuration Register 0
	ldr r1, =0x7F008800 					
	ldr r0, =0x11110000	//0001 0001 0001 0001 0000 0000 0000 0000 b;															  
						//GPK4, 位[19:16], 值为0001, 配置为Output;
						//GPK5, bit[23:20], 0001; 同理, GPK6,GPK7均为Output;
	str r0, [r1]							

	mov r2, #0x1000		// 循环次数, 0x1000次点亮LED灯;
						// 每次led_blink代码段循环一次，r2自减一;
led_blink:
	// 设置GPKDAT，使GPK_4/5/6/7引脚输出低电平，LED亮
	// GPKDAT - 0x7F008808, Port K Data Register
	ldr r1, =0x7F008808 					
	mov r0, #0
	str r0, [r1]		// GPKDAT全部设为0; 

	// 延时
	bl delay							

	// 设置GPKDAT，使GPK_4/5/6/7引脚输出高电平，LED灭
	ldr r1, =0x7F008808 					
	mov r0, #0xf0		// 1111 0000 b; 配置GPKDAT寄存器的bit[7:4]为高电平;
	str r0, [r1]

	// 延时
	bl delay	
	
	sub r2, r2, #1		// 等价于C语言：r2--; r2自减1; r2在
	cmp r2,#0
	bne led_blink		// 不等于0则继续循环

	// ----4. 执行完程序，死循环----
halt:
	// arm汇编指令: b, 跳转到对应代码函数段;
	b halt				// 死循环 


	//下面的代码段不会自上而下执行到，只能通过B/BL指令调用到;
	// 延时函数delay;
delay:
	mov r0, #0x1000000		//配置循环次数
delay_loop:
	// r0自减一，然后判断是否为0；
	cmp r0, #0				
	sub r0, r0, #1
	bne delay_loop		// 不为0则继续循环
	mov pc, lr			// 为0则跳出循环;

	// bl指令在调用函数前，会先将返回地址保存在lr寄存器中
	// 执行完函数以后，通知配置PC寄存器：mov pc, lr ，就可以实现函数返回。
	// b指令没有此操作;
