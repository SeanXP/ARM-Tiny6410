/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < start.S >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/26 >
    > Last Changed: 	 
    > Description:		FriendlyARM - Tiny6410 裸机程序 - 汇编启动程序

	ARM上电后，先执行start.S, 初始化不同的模块后，调用c语言的main函数;
	// ----0. 异常向量表----
	// ----1. 配置ARM CPU寄存器的外设地址----
	// ----2. 关看门狗----
	// ----3. 设置栈----
	// ----5. 设置时钟----
	// ----6. SDRAM初始化----
	// ----7. 重定位----
	// ----8. 清BSS段----
	// ----9. MMU初始化----
	// ---10. 调用main函数----
	
****************************************************************/
.extern main
.text
.global _start							
	
_start:		//_start, 记录这段汇编代码的开始地址
	//{{{ ----0. 异常向量表----
	// 一般规定中断向量地址放在存储器开头最低1KB空间(地址0~0x3FF)
	// 这里将start.S拷贝到SDRAM开头(0x5000_0000), 并通过MMU配置虚拟地址0映射到0x5000_0000;
	// 由硬件产生的中断标识码被称为中断类型号
	// 中断类型号 x 4 = 存放中断子程序的首地址 = 中断向量
	// 0x0:  复位异常
	b reset          		        	// 复位时,cpu跳到0地址 
	// 0x04: 未定义指令异常
	ldr 	pc, _undefined_instruction  
	// 0x08: 管理模式(SVC), 通过SWI指令进入管理模式
	ldr 	pc, _swi  		          
	// 0x0C: 指令预取终止导致的异常
	//ldr	pc, _prefetch_abort	
	// 0x10: 数据访问终止导致的异常
	//ldr	pc, _data_abort   
	// 0x14: 保留
	//ldr	pc, _not_used
	// 0x18: 中断模式
	//ldr	pc, _irq 
	// 0x1C: 快速中断模式
	//ldr	pc, _fiq   
		// 设计FIR为0x1C, 是为了更加快速地相应FIQ; 因为后面没有其他异常向量表, 因此这里可以将代码存放在0x1c开头的内存中;
	//}}}

reset:
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

	//{{{ ----9. MMU配置----
	bl mmu_init  
	//}}}

	ldr sp, =0x57000000		// 管理模式对应的栈空间, sp=r13_svc=0x57000000
	// {{{ 串口初始化
	bl UART0_Port_Init
	bl UART0_Set_BaudRate
	bl UART0_Set_Format
	// }}}

	// ARM复位后进入管理模式(svc)
	// {{{ 配置CPSR寄存器, 切换为System模式
	mrs r0, cpsr   
	ldr r1, =svc_str
	// r0 为向CPSR写的数据, 可以代表现在的CPSR寄存器的值; r1为字符串首地址;
	bl print_cpsr			// 调用C语言函数, 打印r0,r1的值;

	// 从管理模式(svc)切换为系统模式(sys)
	// CPSR[4:0], 0b10000, 用户模式; 0b10011, 管理模式; 0b11111, 系统模式;
	msr cpsr_c, #0xDF		// 0b1101_1111, [7] IRQ=1,  [6] FIQ=1, [4:0]=0b11111
	// cpsr_c, cpsr_c代表的是这32位中的低8位，也就是控制位
	ldr sp, =0x56000000		// 配置系统模式对应的栈, sp=r13_sys=0x56000000, 如果没有配置, 则无法调用c语言函数

	mrs r0, cpsr   
	ldr r1, =sys_str
	bl print_cpsr 
	// }}}

	/* 
	// {{{ 中断向量表数据打印；结合反汇编文件；
    ldr r1, =0x00000004
	ldr r0, [r1]
	bl UART0_SendData32

    ldr r1, =0x00000008
	ldr r0, [r1]
	bl UART0_SendData32
	//}}}
	*/

	// {{{ 通过SWI汇编指令, 进入管理模式模式(svc), 自动设置CPSR对应模式位M[4:0]=0b10011;
	// CPSR[5] = 0, 所有异常中断均在ARM状态(CPSR[5]=0)下处理
	// 切换到备份寄存器r13_svc, r14_svc
	// 自动把swi的下一条指令地址存到r14(lr)_svc, 自动保存CPSR到SPSR_svc;
	// 触发异常, 异常号为0x2, 对应中断向量地址0x8, 跳转到用户自定义的ISR, IRQ中断禁止位(CPSR的I位为1)
	swi 0
	// swi, software interrupt, 软中断汇编指令
	// 该指令产生一个SWI异常。处理器模式改变为超级用户模式(svc)，CPSR寄存器保存到svc模式下的SPSR寄存器，并且跳转到SWI向量。
	// swi指令后面跟的数字为中断编号立即数; 在中断服务函数中根据此立即数决定不同的功能;
	//}}}

	// {{{ 未定义指令终止异常
undef:
	.word 0xff000000	//错误的汇编指令, 导致未定义指令终止异常
	// cpu进入Undefined模式, 自动设置CPSR对应模式位M[4:0]=0b11011;
	// 切换到备份寄存器r13_und, r14_und
	// 自动把swi的下一条指令地址存到r14(lr)_und, 自动保存CPSR到SPSR_und;
	// 触发异常, 异常号为0x1, 对应中断向量地址0x4, 跳转到用户自定义的ISR 
	// }}}

	//{{{ ---10. 调用main函数----
	ldr pc, =main
	//}}}

halt:
	b halt	

// {{{ 管理模式-SWI中断服务
_swi:
	.word swi
swi:
	// cpu进入svc模式后, 会自动把之前的cpsr寄存器的值保存到spsr_svc寄存器
	// 保存cpsr是自动的，但是执行完SWI中断服务后，需要人工还原cpsr寄存器;

	// 1. 保存现场
	// stmdb汇编指令, 将所选的多个寄存器的值存储在栈中
	// lr(R14)为返回地址, 这里就是swi的下一条指令地址
	stmdb sp!, {r0-r12, lr}

	// 2. 处理异常, 这里就是调用C语言打印CPSR寄存器的值
	mrs r0, cpsr
	ldr r1, =swi_str
	bl print_cpsr

	// 3. 恢复现场
	// ldmia汇编指令, 与stmdb指令对应, 从栈中按顺序将值读取回寄存器
	// '^'表示把spsr恢复到cpsr
	// 这里把之前lr的值赋给PC, 实现跳转;
	ldmia sp!, {r0-r12, pc}^ 		
//}}}

// {{{ 未定义指令异常-中断服务
_undefined_instruction:
	.word undefined_instruction
undefined_instruction:
	ldr sp, =0x55000000		// 未定义模式对应的栈空间, sp=r13_und=0x55000000
	// 1. 保存现场, 防止通用寄存器的值被覆盖
	stmdb sp!, {r0-r12, lr}
	// 2. 处理异常
	mrs r0, cpsr
	ldr r1, =und_str
	bl print_cpsr
	// 3. 恢复现场, 同时恢复cpsr, 则切换ARM的工作模式
	ldmia sp!, {r0-r12, pc}^
//	}}}

// {{{ 字符串und_str/sys_str/und_str的存储位置
und_str:
	.word 0x00646e75  				// 字符串"und" 
	// [0] = 'u'= 0x75
	// [1] = 'n'= 0x6e
	// [2] = 'd'= 0x64
	// [3] = '\0'= 0x00
sys_str:
	.word 0x00737973  				// 字符串"sys"
	// 字符串的首地址为sys_str的地址 
	// [0] = 's'= 0x73
	// [1] = 'y'= 0x79
	// [2] = 's'= 0x73
	// [3] = '\0'= 0x00
swi_str:
	.word 0x00697773  				// 字符串"swi" 
svc_str:
	.word 0x00637673  				// 字符串"svc" 
//}}}
