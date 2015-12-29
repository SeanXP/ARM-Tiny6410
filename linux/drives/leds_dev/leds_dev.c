/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < leds_dev.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/07 >
    > Last Changed: 
    > Description:		Friendly ARM Tiny6410 - Linux Device Drivers - LED - 字符混杂设备miscdevice驱动编写

						对于LED, 实现ioctl()函数就已足够;
						LED I/O 在module_init时配置为输出方向, 
						应用程序用过ioctl发送cmd(0,关闭; 1,打开)和arg(LED编号);

		LED1 -> GPK4 -> GPKCON0[19:16] / GPKDAT[4]
		LED2 -> GPK5 -> GPKCON0[23:20] / GPKDAT[5]
		LED3 -> GPK6 -> GPKCON0[27:24] / GPKDAT[6]
		LED4 -> GPK7 -> GPKCON0[31:28] / GPKDAT[7]
		低电平点亮LED灯
****************************************************************/
// {{{ include file
#include <linux/module.h>		// THIS_MODULE, MODULE_AUTHOR, MODULE_LICENSE...
#include <linux/init.h>			// module_init, module_exit 
#include <linux/fs.h>			// struct file_operations; 系统调用与驱动程序的映射;
								// open(), read(), write(), ioctl() ...
								// 应用程序打开与驱动程序挂钩的设备节点文件，对其进行系统调用，
								// 最终都会执行到file_operations中对应的函数;
#include <linux/miscdevice.h>	// misdevice驱动程序
#include <asm/io.h>				// ioread32,iowrite32
// linux/arch/arm/mach-s3c64xx/include/mach/gpio-bank-[a-q].h
#include <mach/gpio-bank-k.h>	// S3C64XX_GPKCON, S3C64XX_GPKDAT
#include <mach/regs-gpio.h>		//定义了gpio-bank-k中使用的S3C64XX_GPK_BASE
#include <mach/map.h>			//定义了S3C64XX_VA_GPIO 
// }}}

// {{{ Device Name 
// 加载模式后，执行"cat /proc/devices"命令看到的设备名称
#define DEVICE_NAME		"ledsdev"
//}}} 

// {{{ unlocked_ioctl
// long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
// unlocked_ioctl是设备驱动程序中对设备的I/O通道进行管理的函数
// 用户程序通过命令码(cmd)告诉驱动程序想做什么, 至于怎么解释这些命令和怎么实现这些命令,这都是驱动程序要做的事情
// 参数:
//	cmd = 0, 关闭arg指定的LED灯;
//	cmd = 1, 打开arg指定的LED灯;
//	arg, (0~4), 其中0表示所有的LED灯, 1~4分别表示LED1~LED4
// 返回: 
// 	返回0或者错误-EINVAL
static long s3c6410_leds_ioctl(struct file* filp, unsigned int cmd, unsigned long arg)
{
	switch(cmd){
		unsigned tmp;
		case 0://close, 这里close与open在同一段代码里处理; switch结构还要有, 用来扩展以后的case 2, case 3 ...
		case 1://open
		{
			// 判断参数arg
			if(arg > 4)
			{
				//参数错误
				return -EINVAL;		
			}
			else if(arg == 0) // 处理所有的LED灯
			{
				// LED1~4 -> GPKDAT[4:7]
				tmp = ioread32(S3C64XX_GPKDAT);		//读出LED1~LED4所在寄存器的值
				if(cmd == 1)
					tmp &= ~(0xF<<4);				//输出低电平, 点亮LED1~LED4
				else if(cmd == 0)					//若是关闭，则关闭掉LED1~LED4
					tmp |= (0xF<<4);				//输出高电平，熄灭LED1~LED4
				// 将改变的值重新写入对应寄存器
				iowrite32(tmp, S3C64XX_GPKDAT);
			}
			else
			{	
				// 处理单个LED灯
				tmp = ioread32(S3C64XX_GPKDAT);
				// arg = 1, LED1 -> GPKDAT4;
				// arg = 2, LED2 -> GPKDAT5;
				// arg = 3, LED3 -> GPKDAT6;
				// arg = 4, LED4 -> GPKDAT7;
				// arg = n, GPKDAT(3 + arg)
				tmp &= ~(1 << (3+arg));				//清除arg所对应的那一位值
				tmp |= ((!cmd) << (3+arg));			//写入新值, cmd=1, 写入0; cmd=0, 写入1;
				iowrite32(tmp, S3C64XX_GPKDAT);
			}
			return 0;
		}
		default:
			return -EINVAL;
	}

	return 0; //按照程序跳转逻辑, 应该不会执行, 这里只是为了对应static int返回值, 避免编译器报警告;
} // }}}

// {{{ file_operations
static struct file_operations S3C6410_LEDS_FOPS = 
{
										// #define THIS_MODULE (&__this_module); __this_module在编译模块时自动创建;
	.owner  =   THIS_MODULE,    		// 这是一个宏，推向编译模块时自动创建的__this_module变量,
	// 驱动函数映射
	.unlocked_ioctl  = 	s3c6410_leds_ioctl,		// 对于LED, 实现一个ioctl函数就足够;
};
// }}}

// {{{ miscdevice
static struct miscdevice misc  = 
{
	.minor = MISC_DYNAMIC_MINOR,		//动态分配次设备号
	.name = DEVICE_NAME,				//设备名称
	.fops = &S3C6410_LEDS_FOPS,
}; //}}}

// {{{ module init
// 执行insmod命令时就会调用这个函数 
// static int __init init_module(void) 	//默认驱动初始化函数,不用默认函数,就要添加宏module_init()
// __init 宏, 只有静态链接驱动到内核时才有用, 表示将此函数代码放在".init.text"段中; 使用一次后释放这段内存;
static int __init s3c6410_leds_init(void)
{
	//在设备驱动程序注册的时候, 初始化LED1～LED4所对应的GPIO管脚为输出, 并熄灭LED1~LED4
	int ret; 		//用于存放函数调用返回值的变量
	unsigned long tmp;

	//配置LED IO为输出
	tmp = ioread32(S3C64XX_GPKCON);
	tmp &= ~(0xFFFF0000);	// 清零GPKCON0[16:31]位
	tmp |= (0x11110000);	// 设置为输出
	iowrite32(tmp,S3C64XX_GPKCON);//写入GPKCON
	
	//熄灭LED灯
	tmp = readl(S3C64XX_GPKDAT);	// 旧的io读写, 不推荐使用; readb, readw, readl, writeb, writew, writel.
	tmp |= (0xF<<4);		//关闭LED灯
	iowrite32(tmp,S3C64XX_GPKDAT);

	// 注册misc
	// misc_device是特殊的字符设备, 注册驱动程序时采用misc_register函数注册
	// 此函数中会自动创建设备节点, 无需mknod指令创建设备文件
	ret = misc_register(&misc);
	if (ret < 0) {			// register failed.
		printk(DEVICE_NAME " can't register mics device - LED Driver\n");
		return ret;
	}
	printk(DEVICE_NAME" initialized.\n");


	// 字符设备注册的另一种方法，需要自行创建设备节点
	// register_chrdev(), 注册字符设备, 参数为主设备号, 设备名字, file_operations结构
	// 这样, 主设备号就和具体的file_operations结构关联起来
	// 操作主设备为LED_MAJOR的设备文件时，就会调用对应file_operations中的相关成员函数
	// 主设备号可以设为0，表示由内核自动分配主设备号
//	ret = register_chrdev(LED_MAJOR, DEVICE_NAME, &S3C6410_LEDS_FOPS);

	return 0;
} //}}}

//{{{ module exit
// 执行rmmod命令时就会调用这个函数 
// static int __exit cleanup_module(void) 	//默认驱动清除函数
// __exit 宏, 表示将此函数代码放在".exit.data"段中; 静态链接时没有使用,因为静态链接的驱动不能被卸载
static void __exit s3c6410_leds_exit(void)
{
	// 卸载驱动程序
	misc_deregister(&misc);
} //}}}

// 这两行指定驱动程序的初始化函数和卸载函数
module_init(s3c6410_leds_init);
module_exit(s3c6410_leds_exit);
// {{{ Module Description
// 描述驱动程序的一些信息，不是必须的
MODULE_VERSION("0.0.1");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SeanXP#iseanxp+code@gmail.com");
MODULE_DESCRIPTION("Tiny6410 LED Driver");
// }}}
