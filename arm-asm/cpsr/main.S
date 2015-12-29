/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < start.S >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/30 >
    > Last Changed:
    > Description:		FriendlyARM - Tiny6410 汇编程序

	交叉编译后, 拷贝到ARM板的linux系统下，作为elf可执行文件运行;
****************************************************************/
.section .text
.global main	// GCC编译器必须将main作为入口，否则无法使用Glibc的库函数(printf)。	
.align 2
	
main:	
	mrs r0, cpsr 
	mrs r1, spsr 
	mov r2, #0 
	bl print			// 调用print(), c语言函数

	movs r0, #1			// 先消除CPSR的Zero位，防止下面的影响无法观测

	mov r0, #0			/* without 's' */
	mrs r1, cpsr
	mov r0, #0    
	mrs r2, cpsr	// r2的值与r1的值相同, mov指令不应该cpsr寄存器
	bl print

	movs r0, #1			// 先消除CPSR的Zero位，防止下面的影响无法观测

	mov r0, #0			/* without 's' */
	mrs r1, cpsr
	movs r0, #0    /* with s, 带s的指令会修改cpsr寄存器 */
	mrs r2, cpsr	// r2的值与r1的值不同了, Zero标志位被设置
	bl print

	mov r0, #0
	bl quit				// 正常退出main函数, 将控制权重新交给操作系统;
