/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < key.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/08/12 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 裸机程序 - 按键驱动
****************************************************************/


#ifndef __key__
#define __key__

// 配置key的相关GPIO端口为输入;
void key_init(void);

// 返回按键的值(0 / 1);
// 在Tiny6410中，低电平表示被按下;
char key_1();
char key_2();
char key_3();
char key_4();

// 配置key的相关GPIO端口为中断引脚;
void key_interrupt_init(void);

// Key1~4外部中断服务函数 - VIC0 - INT_EINT0 - External interrupt Group 0: 0 ~ 3
void key_irq_handler(void);

#endif
