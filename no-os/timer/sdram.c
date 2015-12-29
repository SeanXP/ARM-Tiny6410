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
#include "sdram.h"
#include "common.h"

// {{{ 根据6410手册P192页相关步骤和sdram手册来初始化dram控制器(dramc)
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
	// 由芯片手册得知(8k/64ms)，即8k行的刷新周期为64ms，则每行刷新周期为64ms/8k=7.8125us;
	VI_SET_VAL(P1REFRESH, NS_TO_CLOCKPERIOD(7800));					//刷新周期:(7.8us)/((1/HCLK)s)=(7.8*10^3)/(1/133*10^6)
		// REFRESH PERIOD REGISTER, 32-bit DRAM controller refresh period register, P1REFRESH
		// [14:0], Refresh period, Memory refresh period in memory clock cycles.

	/* 2.2 时间参数，下列设置全都是取了最小值, 最小值依然可以正常运行; */
	//CAS Latency:  (Column Address Strobe latency), 列地址选通脉冲时间延迟, 指的是内存存取数据所需的延迟时间;
	//				简单来说，就是内存接到CPU的指令后的反应速度。一般的参数值是2和3两种。数字越小，代表反应所需的时间越短。
	// 				CL=2所表示的意义是此时内存读取数据的延迟时间是两个时钟周期。
	//				当CL=3时，内存读取数据的延迟时间就应该是三个时钟周期，因此，这“2”与“3”之间的差别就不仅仅局限于“1”了，而是1个时钟
	//				工作在相同频率下的同种内存，将CL设置为2会得到比3更优秀的性能（当然你的内存必须支持CL=2的模式）。
	//				芯片手册K4X2G323PD-8GD8_90F_8x13_R10.pdf中, CAS Latency (2, 3)
	VI_SET_VAL( P1CASLAT, ( 3 << 1 ) );  
		// CAS LATENCY REGISTER, 32-bit DRAM controller CAS latency register, P1CASLAT (reset value -> 0b110)
		// [0], CAS Half cycle, 0 = Zero cycle offset to value in [3:1]. 1 = Half cycle offset to the value in [3:1].
		// [3:1], CAS Latency, CAS latency in memory clock cycles.

	// tCK, Clock cycle time, CL=2, min(tCK) = 12ns; CL=3, min(tCK) = 5ns;

	// 下列设置均在sdram手册中可查询到( K4X2G323PD-8GD8_90F_8x13_R10.pdf - 13.0 AC TIMMING PARAMETERS & SPECIFICATIONS )
	// 由芯片手册可知, t_DQSS, CK to valid DQS-in, t_DQSS = (0.75 ~ 1.25 tCK);
	VI_SET_VAL( P1T_DQSS, 0x1 );		
		// T_DQSS REGISTER, 32-bit DRAM controller t_DQSS register, P1T_DQSS (reset value->0x1)
		// [1:0], t_DQSS, Write to DQS in memory clock cycles.;
		// [31:2], Read undefined. Write as Zero;

	// t_MDR, Mode register set cycle time, min(t_MDR) = 2 tCK;
	VI_SET_VAL( P1T_MRD, 0x2 );							
		// T_MRD REGISTER, 32-bit DRAM controller t_MRD register, P1T_MRD (reset value-> 0x02)
		// [6:0], t_MRD, Set mode register command time in memory clock cycles.

	// t_RAS, Row active time, t_RAS = (40 ~ 70000)ns;
	VI_SET_VAL( P1T_RAS, NS_TO_CLOCKPERIOD(40) );					
		// P1T_RAS, T_RAS REGISTER, 32-bit DRAM controller t_RAS register (reset value -> 0x7)
		// [3:0], t_RAS, Set RAS to precharge delay in memory clock cycles.;

	// t_RC, Row cycle time, min(t_RC) = 55ns;
	VI_SET_VAL( P1T_RC, NS_TO_CLOCKPERIOD(55) );		
		// P1T_RC, T_RC REGISTER, 32-bit DRAM controller t_RC register (reset value -> 0xB)
		// [3:0], t_RC, Set Active bank x to Active bank x delay in memory clock cycles.

	unsigned int trcd = NS_TO_CLOCKPERIOD( 15 );							
	// t_RCD, RAS to CAS delay, min(t_RCD) = 15ns; 
	VI_SET_VAL( P1T_RCD, trcd | (( trcd - 3 ) << 3 ) );
		// P1T_RCD, T_RCD REGISTER, 32-bit DRAM controller t_RCD register (reset value -> 0x1D)
		// [2:0], t_RCD, Set the RAS to CAS minimum delay in memory clock cycles ( Initial State 3b101 )
		// [5:3], scheduled_RCD, Set t_RCD-3. ( Initial State 3b011 )

	unsigned int trfc = NS_TO_CLOCKPERIOD( 120 );
	// t_RFC, Auto refresh cycle time, min(t_RFC) = 120 ns;
	VI_SET_VAL( P1T_RFC, trfc | ( ( trfc-3 ) << 5 ) );   
		// P1T_RFC, 32-bit DRAM controller t_RFC register, (reset value -> 0x212)
		// [4:0], t_RFC, Set the autorefresh command time in memory clock cycles ( initial state 0x12)	
		// [9:5], scheduled_RFC, Set t_RFC - 3. ( initial state 0x10)

	unsigned int trp = NS_TO_CLOCKPERIOD( 15 );
	// t_RP, Row precharge time, min(t_RP) = 15 ns;
	VI_SET_VAL( P1T_RP, trp | ( ( trp - 3 ) << 3 ) ); 
		// P1T_RP, 32-bit DRAM controller t_RP register, (reset value -> 0x1D)
		// [2:0], t_RP, Set the precharge to RAS delay in memory clock cycles ( 3b101 )
		// [5:3], scheduled_RP, Set t_RP -3. (3b011)

	//t_RRD, Row active to Row active delay, min(t_RRD) = 10ns;
	VI_SET_VAL( P1T_RRD, NS_TO_CLOCKPERIOD(10) );
		// P1T_RRD, 32-bit DRAM controller t_RRD register (reset value -> 0x2)
		// [3:0], t_RRD, Set Active bank x to Active bank y delay in memory clock cycles. (0x2)

	// t_WR, Write recovery time, min(t_WR) = 12ns;
	VI_SET_VAL( P1T_WR, NS_TO_CLOCKPERIOD(12) );
	
	VI_SET_VAL( P1T_WTR, 0x1 );
	VI_SET_VAL( P1T_XP, 0x1 );
	// t_XSR, Exit self refresh to active command, min(t_XSR) = 120ns;
	VI_SET_VAL( P1T_XSR, NS_TO_CLOCKPERIOD(120) );
	VI_SET_VAL( P1T_ESR, NS_TO_CLOCKPERIOD(120) );
	
	/* 2.3 chip configuration */
	// P1MEMCFG, 32-bit DRAM controller memory config register (reset value -> 0x01_0020)
	//		[2:0], Column bits; 000 = 8 bits, 001 = 9 bits, 010 = 10 bits, 011 = 11 bits, 100 = 12 bits;
	VI_SET_NBIT( P1MEMCFG, 0, 3, 0x2 );  					// column address(10): A0~A9
	//		[5:3], Row bits; 000 = 11 bits, 001 = 12 bits, 010 = 13 bits, 011 = 14 bits, 100 = 15 bits, 101 = 16 bits;
	VI_SET_NBIT( P1MEMCFG, 3, 3, 0x3 );  					// row address(14): A0~A13
	//		[6], AP bit, Encodes the position of the auto-precharge (自动预充电) bit in the memory address;
	//					 设置地址中的哪一位来控制Auto-Precharge. K4X2G323PD为A10;
	//					 0 = address bit 10; 1 = address bit 8.
	VI_SET_BIT_ZERO( P1MEMCFG, 6 );		  					// A10/AP 
	//		[17:15], Memory burst, 000 = Burst 1, 001 = Burst 2, 010 = Burst 4, 011 = Burst 8, 100 = Burst 16;
	//		This value must also be programmed into SDRAM mode register using the DIRECTCMD register and must match it.
	//		突发（Burst）是指在同一行中相邻的存储单元连续进行数据传输的方式，连续传输的周期数就是突发长度（Burst Lengths，简称BL）
	VI_SET_NBIT( P1MEMCFG, 15, 3, 0x2 ); 					//  Burst Length (2, 4, 8, 16)
	
	// P1MEMCFG2, 32-bit DRAM controller configuration register (reset value -> 0x0B45)
	//		[0], sync, Set high when aclk and mclk are synchronous.	(reset value -> 0b1)
	//		[1], a_gt_m_sync, Requires to be set HIGH when running the aclk and mclk;(reset value -> 0b0)
	//		[2], DQM init, Sets the level for the dqm outputs after reset.(reset value -> 0b1)
	// 		[3], cke_init, Sets the level for the cke outputs after reset. 
	VI_SET_NBIT( P1MEMCFG2, 0, 4, 0x5 );					// 0b0101
	// 		[7:6], Memory width, 00=16-bit, 01=32-bit;
	VI_SET_2BIT( P1MEMCFG2, 6, 0x1 );						// 32 bit 
	//		[10:8], Memory type, 000 = SDR SDRAM, 001 = DDR SDRAM, 011 = Mobile DDR SDRAM;	
	VI_SET_NBIT( P1MEMCFG2, 8, 3, 0x3 );					// Mobile DDR SDRAM 	
	//		[12:11], Read delay, 00 = Read delay 0 cycle (usually for SDR SDRAM. The SDR configuration requires read_dealy set to zero.)
	//							 01 = Read delay 1 cycle (usually for DDR SDRAM and mobile DDR SDRAM)
	//							 10, 11 = Read delay 2 cycle
	VI_SET_2BIT( P1MEMCFG2, 11, 0x1 );

	// P1_chip_0_cfg, 32-bit DRAM controller chip_<n>_cfg register 
	// 		[7:0], Address mask, ( reset value -> 0x00 ) 
	//		[15:8], Address match, (reset value -> 0xFF ) 
	// 		[16], BRC_RBC, memory organization ; 0 = Row-Bank-Column organization, 1 = Bank-Row-Column organization.
	VI_SET_BIT_ONE( P1_chip_0_cfg, 16 );				// Bank-Row-Column organization 


	/* 3. 初始化sdram */
	// 5.4.3 DDR/MOBILE DDR SDRAM INITIALIZATION SEQUENCE

	// [19:18], Memory command, Determines the command required;
	// [22], Extended Memory command 
	//		3'b000 = Prechargeal
	//		3'b001 = Autorefresh
	//		3'b010 = Modereg or Extended modereg access
	//		3'b011 = NOP
	//		3'b100 = DPD (Deep Power Down)
	VI_SET_VAL( P1DIRECTCMD, 0xc0000 ); 					// NOP, 		1100 0000 0000 0000 0000
	VI_SET_VAL( P1DIRECTCMD, 0x00000 );						// precharge
	VI_SET_VAL( P1DIRECTCMD, 0x40000 );						// auto refresh
	VI_SET_VAL( P1DIRECTCMD, 0x40000 );						// auto refresh
	// Extended Mode Register Set (EMRS)
	// The extended mode register is designed to support for the desired operating modes of DDR SDRAM.
	VI_SET_VAL( P1DIRECTCMD, 0xa0000 ); 					// EMRS,		1010 0000 0000 0000 0000 
															// [17:16] = 	  10, BA1 = 1, BA0 = 0;
															// [7:5]   = 		           000, Driver Strength = full;
															// [2:0]   =                         000, PASR = Full Array;
	// Mode Register Set (MRS) 
	// The mode register is designed to support the various operating modes of Mobile DDR SDRAM.
	// MRS寄存器设计用来支持多种操作模式下的Mobile DDR SDRAM.
	// It includes Cas latency, addressing mode, burst length, test mode and vendor specific options to make Mobile DDR SDRAM useful for variety of applications.
	// Two clock cycles are required to complete the write operation in the mode register.
	// This command must be issued only when all banks are in the idle state.
	// MRS模式下的寄存器配置有固定的要求: K4X2G323PD-8GD8_90F_8x13_R10.pdf - 8.1 Mode Register Set (MRS)
	VI_SET_VAL( P1DIRECTCMD, 0x80032 ); 	// MRS, 		1000 0000 0000 0011	0010
											//      [17:16] = 00, BA1 = 0, BA0 = 0; 
											//     	[6:4]   =               011, CAS Latency, 3; 必须和P1CASLAT寄存器的配置相同
											//		[3]     =                   0, Burst Type; 0-Sequential, 1-Interleave; 
											//		[2:0]	=                    010, Burst Length; 010, 4;
	// 注意在设置EMRS/MRS寄存器时，不能分开赋值。下面是错误的代码:
		// VI_SET_NBIT( P1DIRECTCMD, 18, 2, 0x0); /* EMRS */
		// VI_SET_NBIT( P1DIRECTCMD, 16, 2, 0x2); /* EMRS Bank Addr */
		// VI_SET_NBIT( P1DIRECTCMD, 0, 13, 0x0);


	// S3C6410的DRAM控制器是基于 ARM PrimeCell CP003 AXI DMC(PL340)，
	// S3C6410的存储器端口0并不支持DRAM，所以只能选用存储器端口1（DMC1）。
	// S3C6410的DMC1基址ELFIN_DMC1_BASE的值为0x7e00_1000。
	// 当DMC1使用32位数据线DRAM时，需要配置MEM_SYS_CFG寄存器，将芯片管脚Xm1DATA[31:16]设置为DMC1的数据域。
	VI_SET_VAL( MEM_SYS_CFG, 0x0 );
	//	MEM_SYS_CFG, [7], ADDR_EXPAND, Set usage of Xm1DATA[31:16] pins.
	//		0 = Xm1DATA[26:16] pins are used for DMC1 upper halfword data field, data[26:16].
	//		1 = Xm1DATA[26:16] pins are used for SROMC upper 11-bit address field, address[26:16].
					
	/* 4. 使dramc进入"ready"状态	*/
	VI_SET_VAL( P1MEMCCMD, 0x000 );

	// [1:0], Controller status, 00 = Config, 01 = Ready, 10 = Paused, 11 = Low-Power;
	while( !(( VI_READ_VAL( P1MEMSTAT ) & 0x3 ) == 0x1));// 等待dramc进入"ready"状态		
} // }}}
