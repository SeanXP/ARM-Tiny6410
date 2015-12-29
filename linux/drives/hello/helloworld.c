/*************************************************************************
    > File Name: helloworld.c
    > Author: SeanXP
    > Mail: iseanxp+code@gmail.com 
    > Created Time: 2013年10月29日 星期二 09时43分51秒
    > Last Changed: 
    > Notes:编写一个内核模块helloworld.当用insmod命令加载模块后,会显示
	>		Hello World...
	>		可以使用lsmod命令显示模块信息,可以用rmmod命令删除该模块. 
	>		lsmod / insmod / rmmod
	>		dmesg可查看从内核发出的消息.
*************************************************************************/

#include<linux/init.h>
#include<linux/module.h>                 //包含对模块的结构定义以及模块的版本控制.
#include<linux/kernel.h>

static int hello_init(void)				//参数为空但是没有void时，编译会警告
{
	printk(KERN_ALERT "Hello World enter the hello module\n");
	return 0;
}

static void hello_exit(void)                //参数为空但是没有void时，编译会警告
{
	printk(KERN_ALERT "Hello World exit the hello module \n");
}

module_init(hello_init); //模块注册
module_exit(hello_exit);

MODULE_LICENSE("DUAL BSD/GPL");          //告诉内核模块的版权信息/许可权限
MODULE_AUTHOR("Sean");
MODULE_DESCRIPTION("A simple Hello Module");
MODULE_ALIAS("simplest module"); //别名
