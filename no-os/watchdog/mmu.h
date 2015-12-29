/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < mmu.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/28 >
    > Last Changed: 
    > Description:		Friendly ARM - Tiny6410 - MMU配置
****************************************************************/


#ifndef __mmu__
#define __mmu__

// Translation Table, 分配一块内存(可自由分配), 用作地址映射, 
// Translation Table Base (TTB), 转换表首地址, 要储存到协处理器C15的Control Register 2(页表基址寄存器)
// TTB的地址存放在页表基址寄存器的[31:14]; [14:0]默认为0;
#define MMU_BASE  0x51000000

// {{{ MMU 寄存器定义
// Control Register 2 : 0101 0001 0000 0000 0000 0000 0000 0000
//						XXXX XXXX XXXX XXXX XX00 0000 0000 0000
// No.1		0x5100_0000 
// No.2		0x5100_0004 0101 0001 0000 0000 0000 0000 0000 0100
// No.3		0x5100_0008 0101 0001 0000 0000 0000 0000 0000 1000
// No.4		0x5100_000C 0101 0001 0000 0000 0000 0000 0000 1100
// ....		........... 0101 0001 0000 0000 00.. .... .... ..00 	([1:0] = 2b00, 地址为4字节对齐)
// No.4096	0x5100_3FFC	0101 0001 0000 0000 0011 1111 1111 1100
//
// TT共有4096(4k)个页表条目, 一个条目4字节/32位; 
// 整个一级页表共占4K x 4Bytes = 16K Bytes空间; Address: 0x5100_0000 ~ 0x5100_3FFF;
// TT(一级页表), 有四种类型的条目(描述符格式):
// 					1. 数据[1:0] = 0b00; Fault, 无效描述符;
//					2. 数据[1:0] = 0b01; Coarse Page Table, 粗页表描述符;
//					3. 数据[1:0] = 0b10; Section Table, 段描述符;
//					4. 数据[1:0] = 0b11; Find Page Table, 细页表描述符;
// 注: 数据[1:0]不是地址[1:0], 地址[1:0]都是0b00(必须4字节对齐);

// 一级页表描述格式 - 段(Section)
// [31:20], Section Base Address, 段表首地址;
// [11:10], AP, 权限检查模式;
#define MMU_SECTION_FULL_ACCESS       (3 << 10)		// AP=0b11, 访问权限:所有模式下允许任何读写访问
// [8:5], Domain, 权限检查域;
#define MMU_SECTION_DOMAIN            (0 << 5)		// 配置为域0 (由CP15-C3寄存器划分16个权限域) 
// [4], 1, 固定为bit 1;
#define MMU_SECTION_SPECIAL           (1 << 4)		// bit 4必须是1
// [3], Ctt, 是否可缓存; 1, Cached; 0, Non-Cached;
#define MMU_SECTION_CACHEABLE         (1 << 3)		// Cached   
// [2], Btt, 写模式;   Ctt=1, Btt=0, 缓存且写穿式(Write Through); Ctt=1, Btt=1, 缓存且回写式(Write Back);
#define MMU_SECTION_BUFFERABLE        (1 << 2)   	// 使能write buffer
// [1:0], 描述符标识, 段描述符为0b10;
#define MMU_SCETION_SECDESC           (2 << 0)      // 段描述符 0b10

// Ctt=1, Btt=1, Cached & Write Back;
#define MMU_SECTION_WB		(MMU_SECTION_FULL_ACCESS | MMU_SECTION_DOMAIN | MMU_SECTION_SPECIAL | MMU_SECTION_CACHEABLE | MMU_SECTION_BUFFERABLE | MMU_SCETION_SECDESC)

// Ctt=0, Btt=0, Non-Cached & Non-Buffered;
#define MMU_SECTION_NCNB	(MMU_SECTION_FULL_ACCESS | MMU_SECTION_DOMAIN | MMU_SECTION_SPECIAL | MMU_SCETION_SECDESC)

// Data Cache / Instruction Cache ENABLE
#define ENABLE_DCACHE_ICACHE  0
#define REGISTER_USE_CACHE    0
// }}}

// 初始化mmu
void mmu_init(void);

// MMU启动
void mmu_enable(void);

// MMU一级页表初始化
void create_page_table(void);
#endif
