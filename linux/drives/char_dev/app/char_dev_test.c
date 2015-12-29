/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < char_dev_test.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/08 >
    > Last Changed: 
    > Description:		测试chr_dev字符驱动的主程序 
						Usage: 
						
						./char_dev_test read
						./char_dev_test write "hello"
						./char_dev_test ioctl clear
							
****************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define CHR_DEV_NAME	"/dev/char_dev" //自己创建的字符驱动设备节点
#define	BUFFER_SIZE		1024

// ioctl cmd
#define	BUFFER_CLEAR_CMD	0x909090		//IO ctrl command, 用于清空设备缓存

char buffer[BUFFER_SIZE];		// 用户空间的Buffer, 存储从内核空间buffer中读取出来的数据;

int write_chr(const char *string);
int read_chr();
int ioctl_chr(int cmd, int arg);

int main(int argc, char *argv[])
{
	if((argc == 3) && (strcmp(argv[1],"write") == 0))
	{
		write_chr(argv[2]);
	}
	else if((argc == 2) && (strcmp(argv[1], "read") == 0))
	{
		read_chr();
	}
	else if((argc == 3) && (strcmp(argv[1], "ioctl") == 0))
	{
		if(strcmp(argv[2], "clear") == 0)
			ioctl_chr(BUFFER_CLEAR_CMD, 0);
	}
	else
	{
		fprintf(stderr, "Usage: %s write 'some_strings'\nUsage: %s read\n", argv[0], argv[0]);
		return -1;
	}

    return 0;
}

int write_chr(const char *string)
{
	int fd = open(CHR_DEV_NAME, O_RDWR | O_NDELAY);	//这里要对设备文件进行读写，必须要有相应的权限
	//O_NDELAY & O_NONBLOCK都使I/O变得非搁置模式,在读取不到数据或写入缓冲区已满会马上返回，不会搁置程序动作。
		
	if(fd < 0)
	{
		fprintf(stderr, "Failed to open device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	if(write(fd,string,strlen(string)) == -1)
	{
		fprintf(stderr, "Failed to write into device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	printf("write into device %s !\n", CHR_DEV_NAME);
	close(fd);
	return 0;
}


int read_chr()
{
	int fd = open(CHR_DEV_NAME, O_RDONLY | O_NDELAY);	
		
	if(fd < 0)
	{
		fprintf(stderr, "Ftiled to open device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	if(read(fd,buffer,BUFFER_SIZE) == -1)
	{
		fprintf(stderr, "Failed to read from device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	printf("read device%s :\n", CHR_DEV_NAME);
	printf("%s\n",buffer);

	close(fd);
	return 0;
}

int ioctl_chr(int cmd, int arg)
{
	int fd = open(CHR_DEV_NAME, O_RDONLY | O_NDELAY);	
		
	if(fd < 0)
	{
		fprintf(stderr, "Ftiled to open device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	if(ioctl(fd, cmd, arg) == -1)
	{
		fprintf(stderr, "Failed to ioctl device %s!\n", CHR_DEV_NAME);
		return -1;
	}
	printf("ioctl ( %d, %d)\n", cmd, arg);

	close(fd);
	return 0;
	
}
