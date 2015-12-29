/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < start.S >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/26 >
    > Last Changed: 	 < 2015/10/27 >
    > Description:		FriendlyARM - Tiny6410 裸机程序 - 汇编启动程序

	ARM上电后，先执行start.S, 初始化不同的模块后，调用c语言的main函数;
	// ----1. 配置ARM CPU寄存器的外设地址----
	// ----2. 关看门狗----
	// ----3. 设置栈----
	// ----5. 设置时钟----
	// ----6. SDRAM初始化----
	// ----7. 重定位----
	// ----8. 清BSS段----
	// ----9. MMU初始化----
	// ---10. IRQ栈指针配置与初始化----
	// ---11. 调用main函数----
	
	向量中断式 - IRQ中断(使用VIC)
****************************************************************/
.extern main
.text
.global _start							
.global key_handle_asm
	
_start:		//_start, 记录这段汇编代码的开始地址
	//{{{ ----1. 配置ARM CPU寄存器的外设地址----
	// 把外设的基地址告诉CPU
    ldr r0, =0x70000000 					//对于6410来说,内存(0x00000000～0x60000000),外设(0x70000000-0x7fffffff)
    orr r0, r0, #0x13						//外设大小:256M
	// 在基于ARM的嵌入式应用系统中，存储系统通常是通过系统控制协处理器CP15完成的。
	// CP15包含16个32位的寄存器，其编号为0~15。
	// MCR : ARM寄存器到协处理器寄存器的数据传送
    mcr p15,0,r0,c15,c2,4       			//把r0的值(包括了外设基地址+外设大小)告诉cpu
	//}}}
    
	//{{{ ----2. 关看门狗----
	//	Watch Dog - WDCON寄存器 - 0x7E004000 , Watchdog timer control register
	ldr r0, =0x7E004000
	mov r1, #0
	str r1, [r0] 							// 将r0寄存器的值写入地址值为r1的（存储器）内存中
	//}}}

	//{{{ ----3. 设置栈----
	// C语言需要栈;
	// 栈有三个作用，包括：
	//		1. 保存现场；
	//		2. 传递参数: 汇编代码调用C函数时，需传递参数；
	//		3. 保存临时变量:包括函数的非静态局部变量以及编译器自动生成的其他临时变量；
	// 设置栈，其实就是设置SP寄存器，让其指向一块可用的内存。
	// SD 启动时，S3C6410的内部8K的SRAM被映射到0X0C000000，而ARM 默认的栈是递减的，所以可以让SP 指向0X0C002000；
	ldr sp, =0x0c002000	//栈可用空间: 0x0c002000 ~ 0x0c000000;
	//}}}

	//{{{ ----5. 设置时钟----
	bl clock_init
	//}}}

	//{{{ ----6. 初始化sdram----
	bl sdram_init
	//}}}

	//{{{ ----7. 重定位----
	// 获得_start的当前地址; 该语句在Stepping Stone与DRAM中运行时得到的地址不同，具有相对性；
	adr r0, _start  	
	ldr r1, =_start			// 获得_start的绝对地址, 在链接时就确定, 这里的r1得到_start的绝对运行地址;	
	ldr r2, =bss_start		// 获得bss_start的绝对地址;
	// 比较r0 / r1的值，若相等，表示代码已在DRAM中，则不需要再拷贝，直接到下一阶段 - BSS段清零;
	cmp r0, r1
	beq clean_bss

	// r0 / r1不相等，则需要拷贝代码至对应地址(从_start段开头拷贝到bss_start段开头);
	// r0, _start的当前运行地址; (在S3C6410的Stepping Stone中)
	// r1, _start的实际运行地址; (内存DRAM)
copy_loop:
	// 将_start到bss_start的内容全部拷贝;
	ldr r3, [r0], #4
	str r3, [r1], #4
	cmp r1, r2
	bne copy_loop
	//}}}
		
	//{{{ ----8. 清BSS段----
	// 没有拷贝bss段, 这里直接把bss_start ~ bss_end的内容设置为0, 作为bss段;
clean_bss:
	// bss_start ~ bss_end 清零
	ldr r0, =bss_start
	ldr r1, =bss_end
	mov r2, #0
	cmp r0, r1		// 若ro,r1相同，则无需清零, 直接跳转至下一个阶段;
	beq on_ddr
clean_loop:
	str r2, [r0], #4
	cmp r0, r1
	bne clean_loop	
	//}}}

on_ddr:
/*
	//{{{ ----9. MMU配置----
	bl mmu_init  
	//}}}
	*/

	//{{{ ---10. IRQ栈指针配置与初始化----
	ldr sp, =0x57000000		// 管理模式对应的栈空间, sp=r13_svc=0x57000000
	// CPSR[4:0], 0b10000, 用户模式; 0b10011, 管理模式; 0b11111, 系统模式;
	msr cpsr_cxsf, #0xd2    // 切换到中断模式IRQ下, 0b1101_0010, [7] IRQ=1,  [6] FIQ=1, [4:0]=0b10010
	ldr sp, =0x56000000     // 设置中断模式栈指针
	msr cpsr_cxsf, #0x13    // 返回管理模式
	// 初始化irq (配置4个按键为中断源，配置中断方式，配置VIC并使能等)
	bl irq_init
	// 开启IRQ中断, 即关闭CPSR的IRQ中断禁止位(写0取消禁止, 写1禁止中断)
	mrs r0, cpsr
	bic r0, r0, #(1 << 7)	// cpsr[7] = 0
	msr cpsr_c, r0
	//}}}
	// main函数执行过程中，可以通过按键来触发IRQ中断
	//{{{ ---11. 调用main函数----
	ldr pc, =main
	//}}}

halt:
	b halt	

// {{{ IRQ中断服务函数 - 汇编版
key_handle_asm:
	// cpu进入irq模式后, 会自动把之前的cpsr寄存器的值保存到spsr_irq寄存器
	// 保存cpsr是自动的，但是执行完中断服务后，需要人工还原cpsr寄存器;
	
	// 不需要计算lr, 此时的lr为lr_irq, 没有什么用; 对于S3C6410, lr_usr的值已经是pc-4, 不需要再更改;
	//sub lr, lr, #4			// 修正返回地址

	// 1. 保存现场
	// stmdb汇编指令, 将所选的多个寄存器的值存储在栈中
	stmdb sp!, {r0-r12, lr}

	// 2. 处理异常, 这里就是调用C语言打印CPSR寄存器的值
	ldr r0, =asm_str
	bl UART0_SendString

	// 3. 恢复现场
	// 中断挂起标志位清零, Each bit is cleared by writing "1";
	ldr r0, =0x7F008924
	mov r1, #15					//EINT0PEND = (0xf);    // EINT0~3
	str r1, [r0]
	// 当前中断执行地址清零
	ldr r0, =0x71200f00 
	mov r1, #0
	str r1, [r0]
	// ldmia汇编指令, 与stmdb指令对应, 从栈中按顺序将值读取回寄存器
	// '^'表示把spsr恢复到cpsr
	// 这里把之前lr的值赋给PC, 实现跳转;
	ldmia sp!, {r0-r12, pc}^ 		

asm_str:
	.word 0x206d7361  				// 字符串"asm irq\n\r\0" 
	// [0] = 'a' = 0x61
	// [1] = 's' = 0x73
	// [2] = 'm' = 0x6d
	// [3] = ' ' = 0x20
	.word 0x0a717269
	// [4] = 'i' = 0x69
	// [5] = 'r' = 0x72
	// [6] = 'q' = 0x71
	// [7] = '\n'= 0x0a			// '\n', 10, 0x0a, LF, 换行
	.word 0x0000000d
	// [8] = '\r'= 0x0d			// '\r', 13, 0x0d, CR, 回车
	// [9] = '\0'= 0x00
//}}}

code_end:
