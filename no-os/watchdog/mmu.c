/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < mmu.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/10/08 >
  > Last Changed: 
  > Description:		Friendly ARM - Tiny6410 - MMU配置
 ****************************************************************/
#include "mmu.h"

// {{{ 初始化mmu
void mmu_init(void)
{
	create_page_table();
	mmu_enable();
} // }}}

// {{{ MMU一级页表初始化
void create_page_table(void)
{
	unsigned long *table = (unsigned long *)MMU_BASE;
	// table[table_index] = MMU_BASE | table_index * 4;
	// 注意C语言中，指针加法的单位与其类型有关;
	// long* 指针+1后的地址为下一个四字节地址;
	// 若table = 0x5100_0000, 则table[100] = 0x5100_0400;
	unsigned long virtul_addr, physical_addr;

	virtul_addr   = 0x00000000;
	physical_addr = 0x00000000;
	while(virtul_addr < 0x80000000) 
	{
		*(table + (virtul_addr >> 20)) = (physical_addr & 0xFFF00000) | MMU_SECTION_NCNB;
		virtul_addr 	+= 0x100000; 
		physical_addr	+= 0x100000;
	}

	// 映射Stepping Stone地址，因为start.S中配置栈顶为0x0c002000; 没有映射则栈无法正常使用
	virtul_addr   = 0x0C000000;
	physical_addr = 0x0C000000;
	// Section base address: [31:20] , 0xFFF0_0000,只取实际地址的前12位放在段描述符中, 因此为physical_addr & 0xFFF0_0000;
	// virtul_addr[31:20] 为Table index, 对应页表的table[13:2]位, 由于C语言指针的运算特性，因此为virtul_addr >> 20;
	*(table + (virtul_addr >> 20)) = (physical_addr & 0xFFF00000) | MMU_SECTION_WB;

	// SDRAM 地址映射(0x5000_0000 ~ 0x6000_0000)
	virtul_addr   = 0x50000000;
	physical_addr = 0x50000000;
	while(virtul_addr < 0x60000000) // 256 MB内存, 一次写一个段描述符, 0x100000(1M空间), 循环256次
	{
		*(table + (virtul_addr >> 20)) = (physical_addr & 0xFFF00000) | MMU_SECTION_NCNB;
		virtul_addr 	+= 0x100000; 
		physical_addr	+= 0x100000;
		// 0x500x_xxxx
		// 0x5FFx_xxxx
	}

	// 外设寄存器地址映射(0x7000_0000 ~ 0x7FFF_FFFF)
	virtul_addr   = 0x70000000;
	physical_addr = 0x70000000;
	while(virtul_addr < 0x80000000) 
	{
		*(table + (virtul_addr >> 20)) = (physical_addr & 0xFFF00000) | MMU_SECTION_NCNB;
		virtul_addr 	+= 0x100000; 
		physical_addr	+= 0x100000;
	}
}//}}}

// {{{ MMU启动
void mmu_enable(void)
{
	unsigned long *table = (unsigned long *)MMU_BASE;

	// ARM体系架构与编程; 嵌入汇编：LINUX内核完全注释
	// ARM体系结构中, 存储系统通常是通过系统控制协处理器CP15完成的
	// CP15包含16个32位的寄存器，其编号为0~15。
	// MCR   ARM寄存器到协处理器寄存器的数据传送
	// MCR指令将ARM处理器的寄存器中的数据传送到协处理器的寄存器中。
	// 如果协处理器不能成功地执行该操作，将产生未定义的指令异常中断。
	// MRC   协处理器寄存器到ARM寄存器的数据传送
	// 参考: http://6xudonghai.blog.163.com/blog/static/336406292008724103317304/
	__asm__(
			"mov    r0, #0\n"
			// 将寄存器r1的值传给p15协处理器的c7寄存器;
			// p15-c7, Cache operations,用于管理指令缓存和数据缓存。 只可写;
			// 对该寄存器的写操作所实现的功能，是通过MCR指令中的opcode_2和CRm两者的组合来选择的，具体组合详见数据手册。
			"mcr    p15, 0, r0, c7, c7, 0\n"    /* 使无效ICaches和DCaches */
			// MCR{cond}   p15,{opcode_1},<Rd>,<CRn>,<CRm>{,opcode_2}
			// MCR2        p15,{opcode_1},<Rd>,<CRn>,<CRm>{,opcode_2}
			// p15, 指定协处理器
			// <cond>为指令执行的条件码, 当<cond>忽略时指令为无条件执行。
			// <opcode_1>为协处理器将执行的操作的操作码。对于CP15协处理器来说， <opcode_1>永远为0b000,当<opcode_1>不为0b000时，该指令操作结果不可预知。 
			// <Rd>作为元寄存器的ARM寄存器，其值被传送到得协处理器寄存器中。 <Rd>不能为PC，当其为PC时，指令操作结果不可预知。 
			// <CRn>作为目标寄存器的协处理器寄存器，其编号可能为C0,C1....,C15。   
			// <CRm>作为"附加的"目标寄存器或者原操作数寄存器，用于区分同一个编号的不同物理寄存器;如果不需要，就将它设置为C0，否则结果不可预知;
			// <opcode_2>提供附加信息，用于区别同一个编号的不同物理寄存器。当指令中指定附加信息时，省略<opcode_2>或者将其指定为0,否则指令操作结果不可预知。

			"mcr    p15, 0, r0, c7, c10, 4\n"   /* drain write buffer on v4 */
			"mcr    p15, 0, r0, c8, c7, 0\n"    /* 使无效指令、数据TLB */
			// 将寄存器r1的值传给p15协处理器的c8寄存器;
			// p15-c8, TLB operations, 管理TLB, 只可写;

			// 将占位符%0的值传给p15协处理器的c2寄存器;
			// p15-c2, Translation table base, 页表基址寄存器;
			// p15-c3, Domain access control, 访问域权限控制器;
			"mov    r4, %0\n"                   /* r4 = 页表基址 */
			"mcr    p15, 0, r4, c2, c0, 0\n"    /* 设置页表基址寄存器 */

			"mvn    r0, #0\n"                   
			"mcr    p15, 0, r0, c3, c0, 0\n"    /* 域访问控制寄存器设为0xFFFFFFFF，
												 * 不进行权限检查 
												 */    

			// 对于控制寄存器，先读出其值，在这基础上修改特定位, 然后再写入;
			"mrc    p15, 0, r0, c1, c0, 0\n"    /* 读出控制寄存器的值 */

			/* 控制寄存器的低16位含义为：.RVI ..RS B... .CAM
			 * R : 表示换出Cache中的条目时使用的算法，
			 *     0 = Random replacement；1 = Round robin replacement
			 * V : 表示异常向量表所在的位置，
			 *     0 = Low addresses = 0x00000000；1 = High addresses = 0xFFFF0000
			 * I : 0 = 关闭ICaches；1 = 开启ICaches
			 * R、S : 用来与页表中的描述符一起确定内存的访问权限
			 * B : 0 = CPU为小字节序；1 = CPU为大字节序
			 * C : 0 = 关闭DCaches；1 = 开启DCaches
			 * A : 0 = 数据访问时不进行地址对齐检查；1 = 数据访问时进行地址对齐检查
			 * M : 0 = 关闭MMU；1 = 开启MMU
			 */

			/*  
			 * 先清除不需要的位，往下若需要则重新设置它们    
			 */
			/* .RVI ..RS B... .CAM */ 
			"bic    r0, r0, #0x3000\n"          /* ..11 .... .... .... 清除V、I位 */
			"bic    r0, r0, #0x0300\n"          /* .... ..11 .... .... 清除R、S位 */
			"bic    r0, r0, #0x0087\n"          /* .... .... 1... .111 清除B/C/A/M */

			/*
			 * 设置需要的位
			 */
			"orr    r0, r0, #0x0002\n"          /* .... .... .... ..1. 开启对齐检查 */
			"orr    r0, r0, #0x0004\n"          /* .... .... .... .1.. 开启DCaches */
			"orr    r0, r0, #0x1000\n"          /* ...1 .... .... .... 开启ICaches */
			"orr    r0, r0, #0x0001\n"          /* .... .... .... ...1 使能MMU */

			"mcr    p15, 0, r0, c1, c0, 0\n"    /* 将修改的值写入控制寄存器 */
			// "%0"和"%1"代表指令的操作数，称为占位符
			// 指令模板后面用小括号括起来的是C语言表达式, (table)在第二个":"后面，对应%1;
			// 在每个操作数前面有一个用引号括起来的字符串(例如"r", "r1"),字符串的内容是对该操作数的限制或者说要求。
			// "r" (table), 表示将C语言的table变量传递给占位符%1, "r"表示需要将table与某个通用寄存器相关联，先将table的值读入寄存器，然后在指令中使用相应寄存器，而不是table本身
			// 如果为"=r" (table), 则步进不仅将table与寄存器关联，读入寄存器；还在最后将寄存器的值写如table变量中。"="表示输出。
			: /* 无输出 */
			: "r" (table) 
			);
	// 嵌入式汇编的一般形式:
	// __asm__ __volatile__ ("<asm routine>" : output : input : modify);
	// 输出部分(output):用以规定对输出变量（目标操作数）如何与寄存器结合的约束(constraint),
	// 输出部分可以有多个约束，互相以逗号分开。每个约束以“＝”开头，接着用一个字母来表示操作数的类型
	// 然后是关于变量结合的约束。
	// 
	// 输入部分(Input):输入部分与输出部分相似，但没有“＝”
	//
	// 修改部分(modify):这部分常常以“memory”为约束条件，以表示操作完成后内存中的内容已有改
	// 如果原来某个寄存器的内容来自内存, 那么现在内存中这个单元的内容已经改变。
} //}}}

