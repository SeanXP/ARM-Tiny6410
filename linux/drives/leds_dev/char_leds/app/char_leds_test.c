/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < char_leds_test.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/14 >
    > Last Changed: 
    > Description:		Friendly ARM Tiny6410 - Linux Device Drivers Test	
						驱动测试程序

						设备节点: /dev/ledsdev
						程序示例: ./leds_dev_test arg cmd
								
 						arg=[0~4], cmd=[0~1]
 						cmd=0表示关闭LED, cmd=1表示打开LED
					 	arg=0, 全关或者全开LED灯
					 	arg=1~4 表示打开或关闭指定的LED灯	
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME		"/dev/ledsdev"

int main(int argc, char **argv)
{
	int on;
	int led_no;
	int fd;

	// 检查led 控制的两个参数, 如果没有参数输入则退出
	if (argc != 3 || sscanf(argv[1], "%d", &led_no) != 1 || sscanf(argv[2],"%d", &on) != 1 ||\
			on < 0 || on > 1 || led_no < 0 || led_no > 4) 
	{
		fprintf(stderr, "Usage: leds led_no 0|1\n");
		exit(1);
	}

	//	打开设备文件
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
	{
		perror("open leds device " DEVICE_NAME);
		exit(1);
	}

	// 通过系统调用ioctl控制led
	ioctl(fd, on, led_no);

	// read
	unsigned long leds_data = 0;
	read(fd, &leds_data, 4);
	printf("read from device: %x\n", leds_data);

	// 关闭设备句柄
	close(fd);

	return 0;
}
