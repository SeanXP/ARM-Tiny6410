/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < hellop.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/12/04 >
    > Last Changed: 
    > Description:		编写一个内核模块hello
						当用insmod命令加载模块后,会显示"Hello World";
						
						使用lsmod命令显示模块信息,rmmod命令删除该模块. 
						dmesg可查看从内核发出的消息. $ demsg | tail 
****************************************************************/

// 内核程序只用使用内核空间特有的头文件，与应用空间的应用程序所用头文件不同;
// 头文件常在"include/linux/xxx", "include/asm/xxx";
#include <linux/kernel.h>
#include <linux/init.h>			//module_init & module_exit
#include <linux/module.h>		//包含对模块的结构定义以及模块的版本控制.
#include <linux/moduleparam.h>	// module_param

// 模块参数定义
static char *whom	=	"world";
static int howmany 	=	1;

// 模块参数宏定义
module_param(whom, charp, S_IRUGO);
module_param(howmany, int, S_IRUGO);
// 支持的默认参数类型: bool, invbool, charp, int, long, short, uint, ulong, ushort.

static int hello_init(void)	//参数为空但是没有void时，编译会警告
{
	int i = 0;
	for(i = 0; i < howmany; i++)
		printk(KERN_ALERT "Hello %s\n", whom);
	// printk, 内核空间的打印函数, 与printf类似, 但无浮点数功能(内核代码未打开浮点支持, 
	// 避免在进出内核空间时保存/恢复浮点处理器的状态, 导致过多额外开销);
	// 内核实现print, 可以在运行时不再依赖C库;
	// 
	// KERN_ALERT, 字符串, 定义消息优先级; 两个字符串可以合并, 因此不能用逗号将KERN_ALERT与后面的字符串分割;
	// 错误: printk(KERN_ALERT, "Hello\n");
	// 
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Hello World exit the hello module \n");
}

module_init(hello_init);	//模块注册
module_exit(hello_exit);	//模块注销

MODULE_AUTHOR("Sean Guo");
MODULE_DESCRIPTION("A simple module");
MODULE_ALIAS("hello module"); 
MODULE_LICENSE("DUAL BSD/GPL");          //告诉内核模块的版权信息/许可权限
