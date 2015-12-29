/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/08/12 >
    > Last Changed: 
	> Description:		C语言实现按键控制, 按键API设计
  						FriendlyARM - Tiny6410 裸机程序 

	功能：按键控制LED灯，四个按键分别点亮LED1,LED2,LED3,LED4;

 ****************************************************************/

#include "leds.h"
#include "key.h"

int main()
{
	leds_init();
	key_init();

	//开始默认关闭LED灯
	leds_OFF_all();

	//CPU轮询
	while(1){
		//轮询法 判断按键是否按下
		if(key_1())			//K1为高电平,没有被按下 
			leds_OFF(1);
		else 
			leds_ON(1);		//低电平, 表示按下按键, 点亮对应LED灯

		if(key_2())
			leds_OFF(2);
		else 
			leds_ON(2);

		if(key_3())
			leds_OFF(3);
		else 
			leds_ON(3);

		if(key_4())
			leds_OFF(4);
		else 
			leds_ON(4);
	}
	return 0;
}
