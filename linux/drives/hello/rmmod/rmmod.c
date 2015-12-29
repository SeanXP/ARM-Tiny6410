/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < rmmod.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/07 >
    > Last Changed: 
    > Description:		解决busybox的rmmod程序问题: 
						卸载模块成功，仍然提示 rmmod: module '模块名' not found    

						编译: 
						arm-linux-gcc -static -o rmmod rmmod.c
						arm-linux-strip -s rmmod

						覆盖ARM板子的rmmod
****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) 
{
	const char *modname = argv[1];
	int ret = -1;

	int maxtry = 10;

	while (maxtry-- > 0) {
		ret = delete_module(modname, O_NONBLOCK | O_EXCL);    //系统调用sys_delete_module
		if (ret < 0 && errno == EAGAIN)          
			usleep(500000);
		else      
			break;
	}

	if (ret != 0)
		printf("Unable to unload driver module \"%s\": %s\n",
				modname, strerror(errno));
}
