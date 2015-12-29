/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < sdram.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/28 >
    > Last Changed: 
    > Description:		ARM-Tiny6410 裸机程序
						DRAM控制器（DRAM CONTROLLER）配置		
						功能: 初始化dram控制器(dramc)
****************************************************************/


#ifndef __sdram__
#define __sdram__

#define HCLK	133000000	// HCLK 133MHZ
// 纳秒转时钟周期, (nanosecond*(10^-9)) / (1/HCLK); +1 用于四舍五入
#define NS_TO_CLOCKPERIOD(nanosecond)		( (nanosecond) / (1000000000 / HCLK) + 1 )

// {{{  DRAM控制器（DRAM CONTROLLER）寄存器地址
#define MEMCCMD		0x7e001004
#define P1REFRESH	0x7e001010
#define P1CASLAT	0x7e001014
#define MEM_SYS_CFG	0x7e00f120
#define P1MEMCFG	0x7e00100c
#define P1T_DQSS	0x7e001018
#define P1T_MRD		0x7e00101c
#define P1T_RAS		0x7e001020
#define P1T_RC		0x7e001024
#define P1T_RCD		0x7e001028
#define P1T_RFC		0x7e00102c
#define P1T_RP		0x7e001030
#define P1T_RRD		0x7e001034
#define P1T_WR		0x7e001038
#define P1T_WTR		0x7e00103c
#define P1T_XP		0x7e001040
#define P1T_XSR		0x7e001044
#define P1T_ESR		0x7e001048
#define P1MEMCFG2	0X7e00104c
#define P1_chip_0_cfg	0x7e001200

// DRAM CONTROLLER STATUS REGISTER, 32-bit DRAM controller status register, P1MEMSTAT
#define P1MEMSTAT	0x7e001000
#define P1MEMCCMD	0x7e001004
#define P1DIRECTCMD	0x7e001008
// }}}

// 根据6410手册P192页相关步骤和sdram手册来初始化dram控制器(dramc)
int sdram_init();
#endif

