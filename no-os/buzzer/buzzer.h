/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < buzzer.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/21 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 - 裸机程序 - 蜂鸣器
****************************************************************/
#ifndef __buzzer__
#define __buzzer__

// 初始化蜂鸣器相关IO
void buzzer_init(void);

// 开启蜂鸣器
void buzzer_ON(void);

// 关闭蜂鸣器
void buzzer_OFF(void);

#endif
