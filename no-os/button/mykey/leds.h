/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < leds.h >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 
  > Description:		C语言实现LED灯控制, LED灯API设计
  						FriendlyARM - Tiny6410 裸机程序 

****************************************************************/


#ifndef __leds__
#define __leds__

// 配置LEDS的相关GPIO端口为输出;
void leds_init(void);

// 让第number个LED灯点亮;
void leds_ON(unsigned short number);

// 让第number个LED灯熄灭;
void leds_OFF(unsigned short number);

// 点亮所有LED灯
void leds_ON_all(void);

// 熄灭所有LED灯
void leds_OFF_all(void);

// 流水灯
// 参数: numbers, 循环次数;
void leds_flowing(unsigned int numbers);

// 二进制加法灯, 按照二进制数字闪烁;
// 参数: numbers, 循环次数;
void leds_binary(unsigned int numbers);

#endif
