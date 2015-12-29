/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < char_dev.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/08 >
    > Last Changed: 
    > Description:		Friendly ARM Tiny6410 - Linux Device Drivers - Char Device
****************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>		// struct file_operations;
#include <linux/cdev.h>		//包含字符驱动设备struct cdev的定义;
#include <asm/uaccess.h>	//copy_to_user() & copy_from_user()

/********************** <linux/cdev.h> *******************
	strut cdev{
		struct kobject kobj;				//内嵌的内核对象
		struct module *owner;				//指向内核模块对象的指针
		const struct file_operations *ops;	//应用程序调用操作系统接口呼叫到设备驱动程序中的相应操作
		struct list_head list;				//链表
		dev_t dev;							//字符设备的设备号
		unsigned int count;					//次设备号的个数
	};
**********************************************************/

#define	DEVICE_NAME	"char_dev"
#define	BUFFER_CLEAR_CMD	0x909090		//IO ctrl command, 用于清空设备缓存

#define	BUFFER_SIZE		1025			//缓存区的大小
static char char_dev_buffer[BUFFER_SIZE] = "20151108";	//默认从设备中读取的字符为20151108;
static int char_dev_string_len = 9;		//记录字符串的长度.

static struct cdev chr_dev;	//定义一个字符设备对象
static dev_t ndev;			//字符设备节点的设备号

static int open_use = 0;	//only one process permited at the same time.

static int chr_open(struct inode *nd, struct file *filp)
{
	int major = MAJOR(nd->i_rdev); //利用MAJOR & MINOR宏操作得到设备的主设备号及次设备号
	int minor = MINOR(nd->i_rdev);
	
	if(open_use == 1)
	{
		printk("Error: some process is opening this device!\n");
		return -1;
	}

	open_use = 1;

	//increse the use count in struct module, 防止在使用途中此模块被卸载
	//try_module_get(chr_dev->owner);	//2.6内核以后无需自身管理

	//打开设备, 打印一条内核信息.
	printk("chr_open(): major=%d,minor=%d\n", major, minor);

	return 0;
}

static int chr_close(struct inode *nd, struct file *filp)
{
	open_use = 0;		//进程占用数清零，表示该设备空闲，其他进程可以打开使用.
	//module_put(chr_dev->owner);	//2.6内核以后无需自身管理
	printk("chr_close():Close the device[%d,%d].\n", MAJOR(nd->i_rdev), MINOR(nd->i_rdev));

	return 0;
}

static ssize_t chr_read(struct file *filep, char __user *buf, size_t count, loff_t *off)
{
	//将缓存数组中的字符串读取出来.

	//判断读取的长度是否有效, 最大只能返回整个数组字符串.
	if(count > char_dev_string_len)
		count = char_dev_string_len;
	copy_to_user(buf, char_dev_buffer, count);
	printk("read %d char from the char_dev succeed!\n", count);

	return 0;
}

static ssize_t chr_write(struct file *filep, const char *buf, size_t count, loff_t *off)
{
	//将写入的字符串存储在缓存数组中.
	
	//判断写入的长度是否有效
	count %= BUFFER_SIZE;
	if(count != 0)
		copy_from_user(char_dev_buffer, buf, count);	//将字符串从用户空间复制到内核空间.
	//因为内核空间与用户空间的内存不能互访，因此需要借用这两个函数进行复制.
	char_dev_string_len = count;

	printk("write %d char into the char_dev succeed!\n", count);

	return count;
}

static long chr_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	//字符设备控制程序, 可以根据给出的命令使得设备执行不同的操作
	
	if(cmd == BUFFER_CLEAR_CMD)
	{
		char_dev_string_len = 0;
		printk("Clear the char_dev to empty!\n");
		return 0;
	}

	return 0;
}


//字符设备驱动程序中一个非常关键的数据结构
struct file_operations chr_ops =
{
	.owner = THIS_MODULE,	//THIS_MODULE是指向当前内核模块对象的指针, owner成员避免在
							//调用file_operations的函数时,所属模块被卸载.
	.open = chr_open,		//设置各个函数入口.
	.release = chr_close,
	.read = chr_read,
	.write = chr_write,
//	.ioctl = chr_ioctl,		//linux kernel 3.x 版本的内核删除了ioctl函数
//	取代的是: long (*unlocked_ioctl) (struct file*, unsigned int, unsigned long);
//			  long (*compat_ioctl) (struct file*, unsigned int, unsigned long);
	.unlocked_ioctl = chr_ioctl,
};

static int chr_init(void)
{
	int ret; //用于存放函数调用返回值的变量

	printk("cdev_init...\n");
	cdev_init(&chr_dev, &chr_ops);	//调用函数初始化字符设备对象
	
	printk("alloc char device id...\n");
	ret = alloc_chrdev_region(&ndev, 0, 1, DEVICE_NAME);	//自动分配设备号
	if(ret < 0)	//分配失败则返回负值
	{
		printk("Failed to alloc id!\n");
		return ret;
	}

	printk("chr_init(): major=%d,minor=%d\n", MAJOR(ndev), MINOR(ndev));

	ret = cdev_add(&chr_dev, ndev, 1);		//将对应的字符设备对象chr_dev注册进系统
	//调用cdev_add把字符设备对象指针嵌入一个struct probe节点中，并将该节点加入
	//cdev_map所实现的哈希链表中. cdev_add 对应 cdev_del函数.
	if(ret < 0)//注册失败
	{
		printk("Failed to add this char device into cdev_map!\n");
		return ret;
	}

	printk("chr_dev: Everything is OK! :-)\n");

	return 0;
}

static void chr_exit(void)
{
	printk("chr_exit(): Removing chr_dev module...\n");
	cdev_del(&chr_dev);	//注销chr_dev对应的字符设备对象
	unregister_chrdev_region(ndev, 1);		//释放分配的设备号
}

module_init(chr_init);
module_exit(chr_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SeanXP");
MODULE_DESCRIPTION("A char device driver as an example.");
