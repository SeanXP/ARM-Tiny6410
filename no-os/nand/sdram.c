/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < sdram.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/09/28 >
    > Last Changed: 
    > Description:		ARM-Tiny6410 裸机程序
						DRAM控制器（DRAM CONTROLLER）配置		
						功能: 初始化dram控制器(dramc)
****************************************************************/
#include "common.h"

#define HCLK	133000000	// HCLK 133MHZ
// 纳秒转时钟周期, (nanosecond*(10^-9)) / (1/HCLK); +1 用于四舍五入
#define NS_TO_CLOCKPERIOD(nanosecond)		( (nanosecond) / (1000000000 / HCLK) + 1 )

//  DRAM控制器（DRAM CONTROLLER）寄存器地址
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

/* 根据6410手册P192页相关步骤和sdram手册来初始化dram控制器(dramc) */
int sdram_init()
{
	// DRAM CONTROLLER INITIALIZATION SEQUENCE
	/* 1. 使dramc进入"config"状态 */
	// Program memc_cmd to ‘3’b100’, which makes DRAM Controller enter ‘Config’ state.
	VI_SET_VAL(P1MEMCCMD, 0x4);
		// DRAM CONTROLLER COMMAND REGISTER, 32-bit DRAM controller command register, P1MEMCCMD
		// [2:0], Memc_cmd, 000 = Go 001 = Sleep 010 = Wakeup 011 = Pause 100 = Configure,101~111=Reserved;
		// [31:3], Undefined. Write as Zero;
	
	/* 2. 设置timing parameter, chip configuration,id configuration registers */
	// Write memory timing parameter, chip configuration, and id configuration registers.
	/* 2.1 刷新周期 */
	//刷新周期:(7.8us)/((1/HCLK)s)=(7.8*10^3)/(1/133*10^6)
	VI_SET_VAL(P1REFRESH, NS_TO_CLOCKPERIOD(7800));					//刷新周期:(7.8us)/((1/HCLK)s)=(7.8*10^3)/(1/133*10^6)
		// REFRESH PERIOD REGISTER, 32-bit DRAM controller refresh period register, P1REFRESH
		// [14:0], Refresh period, Memory refresh period in memory clock cycles.

	/* 2.2 时间参数，下列设置全都是取了最小值 */
	//CAS Latency: 	指的是内存存取数据所需的延迟时间;
	//				简单的说，就是内存接到CPU的指令后的反应速度。一般的参数值是2和3两种。
	//				K4X1G163PQ的芯片手册上CAS Latency=3 
	VI_SET_VAL( P1CASLAT, ( 3 << 1 ) );  
		// CAS LATENCY REGISTER, 32-bit DRAM controller CAS latency register, P1CASLAT (reset value -> 0b110)
		// [0], CAS Half cycle, 0 = Zero cycle offset to value in [3:1]. 1 = Half cycle offset to the value in [3:1].
		// [3:1], CAS Latency, CAS latency in memory clock cycles.

	VI_SET_VAL( P1T_DQSS, 0x1 );		//下列设置均在sdram手册中可查询到				
		// T_DQSS REGISTER, 32-bit DRAM controller t_DQSS register, P1T_DQSS (reset value->0x1)
		// [1:0], t_DQSS, Write to DQS in memory clock cycles.;
		// [31:2], Read undefined. Write as Zero;
	VI_SET_VAL( P1T_MRD, 0x2 );							
		// T_MRD REGISTER, 32-bit DRAM controller t_MRD register, P1T_MRD (reset value-> 0x02)
		// [6:0], t_MRD, Set mode register command time in memory clock cycles.
	VI_SET_VAL( P1T_RAS, NS_TO_CLOCKPERIOD(42) );					
		// P1T_RAS, T_RAS REGISTER, 32-bit DRAM controller t_RAS register (reset value -> 0x7)
		// [3:0], t_RAS, Set RAS to precharge delay in memory clock cycles.;
	VI_SET_VAL( P1T_RC, NS_TO_CLOCKPERIOD(60) );		
		// P1T_RC, T_RC REGISTER, 32-bit DRAM controller t_RC register (reset value -> 0xB)
		// [3:0], t_RC, Set Active bank x to Active bank x delay in memory clock cycles.

	unsigned int trcd = NS_TO_CLOCKPERIOD( 18 );							
	VI_SET_VAL( P1T_RCD, trcd | (( trcd - 3 ) << 3 ) );
	unsigned int trfc = NS_TO_CLOCKPERIOD( 72 );
	VI_SET_VAL( P1T_RFC, trfc | ( ( trfc-3 ) << 5 ) );   
	unsigned int trp = NS_TO_CLOCKPERIOD( 18 );
	VI_SET_VAL( P1T_RP, trp | ( ( trp - 3 ) << 3 ) ); 

	VI_SET_VAL( P1T_RRD, NS_TO_CLOCKPERIOD(12) );
	VI_SET_VAL( P1T_WR, NS_TO_CLOCKPERIOD(12) );
	
	VI_SET_VAL( P1T_WTR, 0x1 );
	VI_SET_VAL( P1T_XP, 0x1 );
	VI_SET_VAL( P1T_XSR, NS_TO_CLOCKPERIOD(120) );
	VI_SET_VAL( P1T_ESR, NS_TO_CLOCKPERIOD(120) );
	
	/* 2.3 chip configuration */
	VI_SET_NBIT( P1MEMCFG, 0, 3, 0x2 );  					// column address(10):A0~A9
	VI_SET_NBIT( P1MEMCFG, 3, 3, 0x3 );  					// row address(14):A0~A13
	VI_SET_BIT_ZERO( P1MEMCFG, 6 );		  					// A10/AP 
	VI_SET_NBIT( P1MEMCFG, 15, 3, 0x2 ); 					//  Burst Length (2, 4, 8, 16)
	VI_SET_NBIT( P1MEMCFG2, 0, 4, 0x5 );
	VI_SET_2BIT( P1MEMCFG2, 6, 0x1 );						// 32 bit 
	VI_SET_NBIT( P1MEMCFG2, 8, 3, 0x3 );					// Mobile DDR SDRAM 	
	VI_SET_2BIT( P1MEMCFG2, 11, 0x1 );
	VI_SET_BIT_ONE( P1_chip_0_cfg, 16 );						// Bank-Row-Column organization 


	/* 3. 初始化sdram */
	VI_SET_VAL( P1DIRECTCMD, 0xc0000 ); 					// NOP
	VI_SET_VAL( P1DIRECTCMD, 0x000 );						// precharge
	VI_SET_VAL( P1DIRECTCMD, 0x40000 );					// auto refresh
	VI_SET_VAL( P1DIRECTCMD, 0x40000 );					// auto refresh
	VI_SET_VAL( P1DIRECTCMD, 0xa0000 ); 					// EMRS
	VI_SET_VAL( P1DIRECTCMD, 0x80032 ); 					// MRS

	VI_SET_VAL( MEM_SYS_CFG, 0x0 );
					
	/* 4. 使dramc进入"ready"状态	*/
	VI_SET_VAL( P1MEMCCMD, 0x000 );
	while( !(( VI_READ_VAL( P1MEMSTAT ) & 0x3 ) == 0x1));// 等待dramc进入"ready"状态		
}
