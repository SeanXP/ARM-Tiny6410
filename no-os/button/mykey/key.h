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

#endif
