/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < start.S >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/08/12 >
    > Last Changed: 
    > Description:	     汇编程序, 用于学习伪指令adr & ldr;	

	adr, ldr都是伪指令：adr是小范围的地址读取伪指令, ldr是大范围的读取地址伪指令。
	实际上adr是将基于PC相对偏移的地址值或基于寄存器相对地址值读取的伪指令，
	而ldr用于加载32位立即数或一个地址到指定的寄存器中。

	用法: 	如果在程序中想加载某个函数或者某个在链接时候指定的地址时(链接地址)请使用adr，
			例如在lds中需要重新定位的地址。
			当加载32位的立即数或外部地址时请用ldr。

****************************************************************/
.global _start						

_start:
	ldr r0, halt
	adr r0, halt
	ldr r0, =halt
	nop

halt:
	// arm汇编指令: b, 跳转到对应代码函数段;
	b halt				// 死循环 
	nop
