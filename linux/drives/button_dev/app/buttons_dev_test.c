/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < buttons_dev_test.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/14 >
    > Last Changed: 
    > Description:		Friendly ARM Tiny6410 - Linux Device Drivers Test	
						驱动测试程序

						设备节点: /dev/buttonsdev
						程序示例: ./leds_dev_test
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME		"/dev/buttonsdev"

int main(int argc, char **argv)
{
	int fd = 0;
	//	打开设备文件
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
	{
		perror("open device " DEVICE_NAME);
		exit(1);
	}

	int i = 0;
	unsigned char key_values[4];
	unsigned char history_values[4];

	// 初始化
	for(i = 0; i < 4; i++)
		history_values[i] = 'r';
	
	//char c;
	//while( (c = getchar()) != EOF)
	while(1)
	{
		i = read(fd, key_values, sizeof(key_values));  
		if(i < 0)
		{
			perror("read device");
		}
		else
		{  
			printf("read key values: <%c,%c,%c,%c>\n", key_values[0], key_values[1], key_values[2], key_values[3]);
			for(i = 0; i < 4; i++)
			{  
				if(history_values[i]=='r' && key_values[i]=='p')
					printf("\t\t\t\t\tButton %d pressed\n", i);  
				else if(history_values[i]=='p' && key_values[i]=='r')  
					printf("\t\t\t\t\tButton %d release\n\n", i);  
				else
					;
				history_values[i] = key_values[i];  
			}  
		} 
	}

	// 关闭设备句柄
	close(fd);

	return 0;
}
