/****************************************************************
  Copyright (C) 2015 Sean Guo. All rights reserved.

  > File Name:         < main.c >
  > Author:            < Sean Guo >
  > Mail:              < iseanxp+code@gmail.com >
  > Created Time:      < 2015/07/31 >
  > Last Changed: 
  > Description:		C语言实现LED灯控制
  						FriendlyARM - Tiny6410 裸机程序 
****************************************************************/

#include "leds.h"

int main()
{
	int number = 0;
	leds_init();

	//开始默认关闭LED灯
	leds_OFF_all();

	// test1
	leds_ON_all();

	// test2
//	leds_flowing(10);
//	leds_OFF_all();

	// test3
//	leds_binary(10);
//	leds_OFF_all();

	return 0;
}
