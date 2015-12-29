/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < common.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/09/29 >
    > Last Changed: 
    > Description:
****************************************************************/


#ifndef __common__
#define __common__

// BIT位掩码宏定义
#define	BIT0		(1<<0)
#define	BIT1		(1<<1)
#define	BIT2		(1<<2)
#define	BIT3		(1<<3)
#define	BIT4		(1<<4)
#define	BIT5		(1<<5)
#define	BIT6		(1<<6)
#define	BIT7		(1<<7)
#define	BIT8		(1<<8)
#define	BIT9		(1<<9)
#define	BIT10		(1<<10)
#define	BIT11		(1<<11)
#define	BIT12		(1<<12)
#define	BIT13		(1<<13)
#define	BIT14		(1<<14)
#define	BIT15		(1<<15)

// 寄存器指针
#define VI *(volatile unsigned int *) 

// 寄存器-特定位操作 (置1/置0)
// eg. VI_SET_BIT_ZERO(A, 1), 寄存器A的BIT1置零;
#define VI_SET_BIT_ZERO(addr, bit)		( (VI addr) &= (~ (1<<(bit)) ) )
#define VI_SET_BIT_ONE(addr, bit)		( (VI addr) |= (  (1<<(bit)) ) )

// 寄存器-特定位操作(指定数据), 先将特定位清零, 然后配置;
// eg. VI_SET_BIT(A, 1, 1), 寄存器A的BIT1置一;
// 	   VI_SET_BIT(A, 1, 0), 寄存器A的BIT1置零;
#define VI_SET_BIT(addr, bit, val)		( (VI addr) = VI_SET_BIT_ZERO(addr, bit) | ((val)<<(bit)) )
// eg. VI_SET_2BIT(A, 1, 1), 寄存器A的[1:2]置0b01;
#define VI_SET_2BIT(addr, bit, val)		( (VI addr) = ( (VI addr) &= (~ (3<<(bit)) ) ) | ((val)<<(bit)) )

// addr, 寄存器地址; bit, 起始比特位数; len, 比特长度; val, 值;
// eg. VI_SET_NBIT(A, 10, 4, 3), 寄存器A的[10:13]置0b0011;
#define VI_SET_NBIT(addr, bit, len, val) \
	( (VI addr) = ( ((VI addr)&(~( ((1<<(len))-1)<<(bit) )) ) | ((val)<<(bit)) ) )
	// 寄存器   = (         清零                              |      置位      )
	// ((1<<(len))-1) << (bit), 为从bit位开始且长度为len的比特掩码
	// eg. bit=4,len=2,最终结果应为[4:5]=0b'11;
	// (1<<len) = 1<<2 = 0b100; 减去1为0b011; 为两位掩码; 再移bit位; 0b110000; 即[4:5]=0b11;


// 寄存器-特定位读取; 返回结果: 0或1;
// eg. VI_GET_BIT(A, 1), 读取寄存器A的BIT1;
#define VI_GET_BIT(addr, bit)			( ((VI addr)&(1<<(bit))) > (bit))
// 获得寄存器的值
#define VI_GET_VAL(addr, val)			( (val) = (VI addr) )

#define VI_READ_VAL(addr)				(VI addr)
#define VI_SET_VAL(addr, val)			((VI addr) = (val))
#define VI_OR_VAL(addr, val)			((VI addr) |= (val))
#define VI_AND_VAL(addr, val)			((VI addr) &= (val))

#endif /* __common__ */
