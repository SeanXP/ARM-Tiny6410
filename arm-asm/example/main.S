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
	mov r0, #12
	mov r1, #34
	mov r2, #56
	bl print			// 调用print(), c语言函数
	mov r0, #0
	bl quit				// 正常退出main函数, 将控制权重新交给操作系统;
