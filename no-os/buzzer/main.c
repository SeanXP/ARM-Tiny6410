/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < main.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/21 >
    > Last Changed: 
    > Description:		Friendly ARM - Tiny6410 - 裸机程序 蜂鸣器
****************************************************************/
#include "buzzer.h"

void delay()
{
	int time;
	for(time = 0; time < 100000; time++)
		;
}

int main()
{
	buzzer_init();
	buzzer_ON();
	delay();
	buzzer_OFF();
	delay();
	buzzer_ON();
	delay();
	buzzer_OFF();
	delay();
    return 0;
}

