/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < buttons_dev.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/11/13 >
    > Last Changed: 
    > Description:		FriendlyARM - Tiny6410 - Linux Device Drivers - 标准字符设备驱动 - Buttons

						需要手动创建设备节点: 
						# insmod buttons_dev.ko
						得到分配的设备号, 假设为253, 则创建设备节点
						# mknod /dev/buttonsdev c 253 0

						LED I/O 在open()时配置为输出方向, 
						应用程序用过ioctl发送cmd(0,关闭; 1,打开)和arg(LED编号);

						K1 -> GPN0 -> GPNCON[1:0] / GPNDAT[0] -> External interrupt Group 0, EINT0
						K2 -> GPN1 -> GPNCON[3:2] / GPNDAT[1] -> External interrupt Group 0, EINT1
						K3 -> GPN2 -> GPNCON[5:4] / GPNDAT[2] -> External interrupt Group 0, EINT2
						K4 -> GPN3 -> GPNCON[7:6] / GPNDAT[3] -> External interrupt Group 0, EINT3

****************************************************************/
// {{{ include file
#include <linux/module.h>		// THIS_MODULE, MODULE_AUTHOR, MODULE_LICENSE...
#include <linux/init.h>			// module_init, module_exit 
#include <linux/fs.h>			// struct file_operations; 系统调用与驱动程序的映射;
#include <asm/io.h>				// ioread32,iowrite32
#include <mach/gpio-bank-n.h>	// 定义了GPNCON
#include <mach/regs-gpio.h>		// 定义了gpio-bank-k中使用的S3C64XX_GPK_BASE
#include <mach/map.h>			// 定义了S3C64XX_VA_GPIO 
#include <linux/cdev.h>			// 包含字符驱动设备struct cdev的定义;
#include <linux/interrupt.h>	// request_irq, free_irq
#include <linux/irq.h>			// flags - IRQ_TYPE_EDGE_FALLING
#include <asm/uaccess.h>   		// copy_to_user() & copy_from_user()
#include <linux/wait.h>			// wait_event_interruptible(wait_queue_head_t q,int condition);
								// wake_up_interruptible(struct wait_queue **q)
								// DECLARE_WAIT_QUEUE_HEAD
// }}}

// {{{ Device Name 
// 加载模式后，执行"cat /proc/devices"命令看到的设备名称
#define DEVICE_NAME		"buttonsdev"
// 设备编号, 为0时会自动分配
#define DEVICE_MAJOR	0
//}}} 

// {{{ cdev
// 每个字符设备都对应一个cdev结构体变量.
// cdev一般它有两种定义初始化方式:静态的和动态的。
// 静态内存定义初始化：
//		struct cdev my_cdev;				// 创建cdev结构体变量
//		cdev_init(&my_cdev, &fops);			// 绑定file_operations
//		my_cdev.owner = THIS_MODULE;		// 配置owner
//动态内存定义初始化:
//		struct cdev *my_cdev = cdev_alloc();
//		my_cdev->ops = &fops;
//		my_cdev->owner = THIS_MODULE;
// 两种使用方式的功能是一样的，只是使用的内存区不一样，一般视实际的数据结构需求而定。
//
/********************** <linux-2.6.38/include/linux/cdev.h> *******************
   strut cdev{
		struct kobject kobj;				//内嵌的内核对象
		struct module *owner;				//指向内核模块对象的指针
		const struct file_operations *ops;	//应用程序调用操作系统接口呼叫到设备驱动程序中相应操作
		struct list_head list;				//链表
		dev_t dev;							//字符设备的设备号
		unsigned int count;					//次设备号的个数
   };
**********************************************************/
static struct cdev buttons_cdev;			//定义一个字符设备对象
static dev_t buttons_dev_t;					//字符设备节点的设备号
// }}}

// {{{ irq_desc - irqaction
//	在ARM体系结构中，Linux使用irq_desc结构体来描述对应的中断, 每个中断号都有一个结构体变量;
// 	部分成员变量: 
//		struct irq_chip		*chip;				//底层的硬件访问
//		irq_flow_handler_t	handle_irq;			//当前中断的处理函数入口
//		struct irqaction	*action;			// IRQ action list , 用户中断处理链表结构体
//			// 驱动程序申请中断时(request_irq()), 会根据参数构建一个irqaction, 添加到对应irq的irq_desc的action链表中;
//			struct irqaction {
//				irq_handler_t handler;		//中断处理函数
//				unsigned long flags;		//标志位，是否要共享中断，触发方式：电平或者是边沿触发
//				void *dev_id;				//传递给中断处理函数handler的参数
//				struct irqaction *next;		//通过*next指针, 连接成链表;
//				int irq;					//中断号
//				const char *name;			//用户注册的中断名称,使用cat /proc/interrupt 查看到的名称
//			} 
//		unsigned int		status;		// IRQ status 当前中断状态
//		const char		*name;//中断名称

// buttons_irq_desc IRQ描述符, 用于给request_irq()函数传递参数;
	// int request_irq(	unsigned int irq, 
	// 					void (*handler)(int irq, void *dev_id, struct pt_regs *regs ), 
	//					unsigned long irqflags, 
	//					const char * devname, 
	//					void *dev_id); 
struct buttons_irq_desc
{
	int irq;				//中断号
	unsigned long flags;	//中断标志号,是否可共享
	char *name;				//中断名称，使用cat /proc/interrupt
	int number;				//中断设备编号, 中断函数中使用, 用于判断哪个按键触发中断;
};

// struct buttons_irq_desc的初始化
// 用来指定所用的外部中断引脚以及中断触发方式、名字
// Tiny6410的按键, 未按位高电平, 按下为低电平;
static struct buttons_irq_desc buttons_irqs[] = 
{
	// IRQ_TYPE_EDGE_BOTH, 上升沿 & 下降沿
	// IRQ_TYPE_EDGE_RISING, 上升沿, 即松开按钮的瞬间;
	// IRQ_TYPE_EDGE_FALLING, 下降沿, 即按下按钮的瞬间;
	{IRQ_EINT(0),	 IRQ_TYPE_EDGE_BOTH,	  "BUTTON0", 0},	  //BUTTON0
	{IRQ_EINT(1),	 IRQ_TYPE_EDGE_BOTH,	  "BUTTON1", 1},	  //BUTTON1	
	{IRQ_EINT(2),	 IRQ_TYPE_EDGE_BOTH,	  "BUTTON2", 2},	  //BUTTON2	
	{IRQ_EINT(3),	 IRQ_TYPE_EDGE_BOTH,	  "BUTTON3", 3},	  //BUTTON3	
};
// }}}

// {{{ global variable

// 按键状态, 由中断函数配置, 用于用户程序查询;
// 初始状态都是released状态'r', 当有按键按下时变为pressed状态'p'
// 数组元素长度要要与按钮数目对应;
static volatile char buttons_values[] = {'r', 'r', 'r', 'r'};  

// 事件监测标志位，用于在read操作中判断是否有按键按下
static volatile int action_flag = 0;

// 声明一个按键的等待队列 
static DECLARE_WAIT_QUEUE_HEAD(buttons_waitq); 
/* 	DECLARE_WAIT_QUEUE_HEAD(name)是一个与等待队列有关的宏 
	展开为: 
	wait_queue_head_t name = 
	{ 
	   .lock=__SPIN_LOCK_UNILOCKED(name.lock), 
	   .task_list={&(name), task_list, &(name).task_list} 
	} 

	其中wait_queue_head_t的定义:
	typedef struct __wait_queue_head wait_queue_head_t; 
	struct __wait_queue_head
	{ 
		spinlock_t lock; 
		struct list_head task_list; 
	}; 
*/

// }}}

// {{{ buttons - interrupt
//static irqreturn_t intr_handler(int irq, void *dev_id, struct pt_regs *regs);
// 得到request_irq()的参数irq, dev_id
//	1. 中断服务程序可能使用到设备结构体中的信息; 在中断处理函数中使用该设备结构体，这是大部分驱动程序通用的一种手法。
//	2. 多个设备共享中断时, 中断处理函数自身需要能识别是否是自己的设备产生了中断。通常这是通过读取该硬件设备提供的中断flag标志位进行判断的。
//		而往往驱动程序里定义的设备结构体通常包含了该设备的IO地址,加上偏移就可以算出中断状态寄存器及中断flag标志位的IO地址信息。
//		中断函数判断是否是自己设备发生中断，从而选择是执行服务程序，还是跳转到下一个irqaction->handler函数中;
//		触发一个irq中断，linux会依次执行同一中断号下所有已注册的中断函数(irq_desc->action->handler);
//  利用dev_id, 多个设备可以共享同一个中断服务函数
static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
	// 这里直接将buttons_irq_desc作为dev_id传递进来;
	struct buttons_irq_desc *buttons_irqs_int = (struct buttons_irq_desc *)dev_id;
	int down;  
	int number;  
	unsigned tmp;  
	unsigned excp = 0;  	// 异常标志位

	// 获得中断设备编号
	number = buttons_irqs_int->number;  
	switch(number) 
	{  
		// 根据不同中断设备编号, 执行不同功能
		case 0:  
			// Button 0 , GPN0, 按下变为低电平;
			tmp = readl(S3C64XX_GPNDAT);
			down = (tmp & (1 << 0));
			break;  
		case 1:  
			// Button 1, GPN1
			tmp = readl(S3C64XX_GPNDAT);  
			down = (tmp & (1 << 1));
			break;  
		case 2:  
			// Button 2, GPN2
			tmp = readl(S3C64XX_GPNDAT);  
			down = (tmp & (1 << 2));
			break;  
		case 3:  
			// Button 3, GPN3
			tmp = readl(S3C64XX_GPNDAT);  
			down = (tmp & (1 << 3));
			break;  
		default:  
			excp = 1;  
			down = 0;  
	}  

	if(excp == 0) // 无异常时
	{   
		// 设置对应的按钮状态变量
		if(down==0)
		{  
			buttons_values[number]='p';
		}
		else
		{  
			buttons_values[number]='r';  
		}  

		printk(DEVICE_NAME " button %d, %c\n", number, buttons_values[number]);
		// 以下两步, 会唤醒可能在s3c6410_buttons_read()中阻塞等待的进程
		action_flag = 1;  						// 中断标志位置位
		wake_up_interruptible(&buttons_waitq);  // 唤醒注册到等待队列上的进程
	}  

	// 中断处理例程应返回一个值指示是否真正处理了一个中断
	// #include <linux/irqreturn.h> 
	/**
	* enum irqreturn
	* @IRQ_NONE        interrupt was not from this device
	* @IRQ_HANDLED     interrupt was handled by this device
	* @IRQ_WAKE_THREAD handler requests to wake the handler thread
	*/
	// #define IRQ_RETVAL(x)   ((x) != IRQ_NONE)
	return IRQ_RETVAL(IRQ_HANDLED);		// IRQ_HANDLED != IRQ_NONE, 则返回true, 表示中断已处理
} // }}}

// {{{ open
// int (*open)(struct inode *inode, struct file *filp)
// open方法提供给驱动程序以初始化的能力，从而为以后的操作完成初始化做准备。
// 初始化字符设备 - Button IO初始化, 中断服务函数注册
static volatile int buttons_press_count[] = {0, 0, 0, 0};	// 记录按键的次数
static int s3c6410_buttons_open(struct inode *nd, struct file *flip)
{
	unsigned long tmp;
	unsigned int buttons_number = sizeof(buttons_irqs) / sizeof(buttons_irqs[0]);
	int err = 0;
	int i = 0; 	//循环变量

	//配置BUTTONS IO为输出
	tmp = ioread32(S3C64XX_GPNCON);
	tmp &= ~(0xff);			// 清零GPNCON0[7:0]位
	tmp |= (0xaa);			// 设置为中断, 0b1010_1010
	iowrite32(tmp, S3C64XX_GPNCON);	//写入GPNCON

	//注册中断服务函数
	for(i = 0; i < buttons_number; i++) //每个按键对应一个外部中断, 都要注册;
	{
		if(buttons_irqs[i].irq < 0)
		{
			// 无效irq, 跳过注册;
			continue;
		}
		// 申请使用IRQ并注册中断处理程序;
		// int request_irq(	unsigned int irq, 
		// 					void (*handler)(int irq, void *dev_id, struct pt_regs *regs ), 
		//					unsigned long irqflags, 
		//					const char * devname, 
		//					void *dev_id); 
		// irq, 要申请的硬件中断号, 固定, 查看CPU的中断部分介绍; 
		// handler, 中断处理函数, 回调函数, 系统将传递下列参数: 
		//		irq, 中断号; dev_id, device id; regs, 寄存器值;
		// irqflags, 中断处理属性: IRQF_DISABLED-FIQ中断函数, IRQF_SHARED-多设备共享中断, 等等;
		// devname, 设置中断名称; 在cat /proc/interrupts中可以看到此名称
		// dev_id, device id, 在中断共享时会用到,一般设置为这个设备的device结构本身或者NULL
		// 		identify which device is interrupting can be set to NULL if the interrupt is not shared.
		// 		dev_id is an input argument and must be globally unique. dev_id每个设备都唯一.
		//		Normally the address of the device data structure is used as the dev_id.
		//		It has value NULL if the interrupt line is NOT shared. It holds relevance only when the interrupt line is being shared.
		// 		When it is shared, this parameter uniquely identifies the interrupt handler on the shared IRQ.
		err = request_irq(buttons_irqs[i].irq, buttons_interrupt, buttons_irqs[i].flags, 
							buttons_irqs[i].name, (void*)(&buttons_irqs[i]));
		if(err != 0)
			break;
		printk(DEVICE_NAME " request_irq: %d, %s \n", buttons_irqs[i].irq, buttons_irqs[i].name);
		/* output:
		   buttonsdev request_irq: 101, BUTTON0
		   buttonsdev request_irq: 102, BUTTON1
		   buttonsdev request_irq: 103, BUTTON2
		   buttonsdev request_irq: 104, BUTTON3
		*/
	}
	if(err) //注册失败, 注销之前注册的中断, 然后退出程序;
	{
		printk(DEVICE_NAME " open() error.\n");
		// 取消之前注册成功的中断;
		i--;	// 第i次没成功, 不需要注销;
		for( ; i >= 0; --i)
		{
			if(buttons_irqs[i].irq < 0) //无效irq, 直接跳过;
				continue;
			disable_irq(buttons_irqs[i].irq); 
			// void free_irq(unsigned int irq,void *dev_id); 
			// 当调用free_irq注销中断处理函数时（通常卸载驱动时其中断处理函数也会被注销掉）
			// 需要dev_id执行删除共享中断线上的哪一个中断; 没有dev_id就无法删除共享中断线的某一中断
			// free_iqr()内部, 通过irq找到irq_desc, irq_desc->action为irqaction链表,
		    //	对比dev_id与irq_desc->action->dev_id, 将对应的action从链表中删除;
			free_irq(buttons_irqs[i].irq, (void*)(&buttons_irqs[i]));
		}
		return -EBUSY;
	}

	// 按钮状态变量复位
	for(i = 0; i < buttons_number; i++)
		buttons_values[i] = 'r';
	// 标志位清零
	action_flag = 0;

	printk(DEVICE_NAME " open().\n");
	return 0;
} // }}}

// {{{ close
static int s3c6410_buttons_close(struct inode *nd, struct file *flip)
{
	// 卸载注册的中断
	int i = 0;
	unsigned int buttons_number = sizeof(buttons_irqs) / sizeof(buttons_irqs[0]);

	for(i = 0; i < buttons_number; i++) 
	{
		if(buttons_irqs[i].irq < 0)	// 错误的irq, 之前跳过注册, 现在也跳过注销;
			continue;
		free_irq(buttons_irqs[i].irq, (void*)(&buttons_irqs[i])); 
		// dev_id用于判断要注销irq的action链表中的哪一个irqaction;
		// dev_id不能传递NULL, 否则遍历到列表末尾扔无法匹配, 报错:"Trying to free already-free IRQ"
	}
	printk(DEVICE_NAME " close().\n");
	return 0;
} // }}}

// {{{ read
// 返回按钮状态表buttons_values[];
static int s3c6410_buttons_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) 
{
	size_t failed_copy_count = 0;
	size_t read_count = min(sizeof(buttons_values), count);

	if (action_flag == 0)	// 标志位未值一，则说明之前尚未发生中断
   	{
		// 没有发生中断, 可以直接返回，也可以阻塞等待按键中断，由设备文件的打开方式决定;
		if (filp->f_flags & O_NONBLOCK)   //设备文件以非阻塞模式打开, 则马上返回  
			return -EAGAIN;  
		else  
			// 阻塞模式, 则等待中断发生; 
			// 当action_flag为真(非零, 即发生中断), 则跳出睡眠;
			wait_event_interruptible(buttons_waitq, action_flag);	// 注册到等待队列中, 等待被唤醒
			// wait_event_interruptible(wq, condition)
			// 将本进程置为可中断的挂起状态, 反复检查condition是否成立，如果成立则退出，如果不成立则继续休眠;
			// 要唤醒此进程(并非直接唤醒, 而是通过调度程序切换上下文, 将此进程调整为可运行状态):
	   		//		1. 设置condition为true
			//		2. 调用wake_up_interruptible(wait_queue_head_t *q)函数
			//		调用wake_up_interruptible时，会检测condition是否为true, 如果不是true, 则再次进行挂起状态;
	}

	// 标志位清零
	action_flag = 0;  

	// 将数据从内核空间拷贝到用户空间
	// Returns number of bytes that could not be copied. 
	failed_copy_count = copy_to_user((void *)buff, (const void *)(&buttons_values), read_count);  
	if(failed_copy_count != 0)
		return -EFAULT;
	else
		return read_count;
} // }}}

// {{{ file_operations
static struct file_operations S3C6410_BUTTONS_FOPS = 
{
	// #define THIS_MODULE (&__this_module); __this_module在编译模块时自动创建;
	.owner = THIS_MODULE,  	// 这是一个宏，推向编译模块时自动创建的__this_module变量
	// 驱动函数映射
	.open = s3c6410_buttons_open,					// 对于字符设备, open()为初始化;
	.release = s3c6410_buttons_close,				// 对于字符设备, close()为关闭设备;
	.read = s3c6410_buttons_read, 					// 返回按钮的状态
};
// }}}

// {{{ module init
static int __init s3c6410_buttons_init(void)
{
	int ret; 		//用于存放函数调用返回值的变量

	printk(DEVICE_NAME " char device init.\n");

	if(DEVICE_MAJOR)	// 设备号不为0, 注册指定设备编号
	{
		// MKDEV(int major,int minor) //通过主次设备号来生成dev_t类型的设备号
		buttons_dev_t = MKDEV(DEVICE_MAJOR, 0);
		// 内核提供了三个函数来注册一组字符设备编号
		// register_chrdev_region(), alloc_chrdev_region(), register_chrdev()
		// 这三个函数都会调用一个共用的__register_chrdev_region() 函数来注册一组设备编号范围
		// int register_chrdev_region(dev_t from, unsigned count, const char *name) - register a range of device numbers
		ret = register_chrdev_region(buttons_dev_t, 1, DEVICE_NAME);	//注册设备编号
		if(ret < 0)		// 注册失败
		{
			printk(DEVICE_NAME " register device id(%d) error.\n", DEVICE_MAJOR);  
			return ret;  
		}
	}
	else				// 设备号为0，自动分配设备号并注册
	{
		// int alloc_chrdev_region(dev_t *dev,unsigned int firstminor,unsigned int count,char *name);
		// firstminor是请求的最小的次编号, count是请求的连续设备编号的总数
		// name为设备名, 返回值小于0表示分配失败
		ret = alloc_chrdev_region(&buttons_dev_t, 0, 1, DEVICE_NAME);  
		if(ret < 0)
		{  
			printk(DEVICE_NAME " alloc char device id error.\n");  
			return ret;  
		}
	}

	{
		int buttons_major = 0;
		buttons_major = MAJOR(buttons_dev_t);	//MAJOR(dev) 获得主设备号  
		printk(DEVICE_NAME " char device major = %d\n", buttons_major);  
	}

	// 已静态创建cdev结构体变量buttons_cdev
	//void cdev_init(struct cdev*cdev,struct file_operations *fops)
	cdev_init(&buttons_cdev, &S3C6410_BUTTONS_FOPS);
	//int cdev_add(struct cdev *dev,dev_t num,unsigned int count)  
	cdev_add(&buttons_cdev, buttons_dev_t, 1); 

	return 0;
} //}}}

//{{{ module exit
static void __exit s3c6410_buttons_exit(void)
{
	// 卸载驱动程序
	cdev_del(&buttons_cdev);						//注销chr_dev对应的字符设备对象
	unregister_chrdev_region(buttons_dev_t, 1);		//释放分配的设备号

	printk(DEVICE_NAME " module exit.\n");
} //}}}

// 这两行指定驱动程序的初始化函数和卸载函数
module_init(s3c6410_buttons_init);
module_exit(s3c6410_buttons_exit);

// {{{ Module Description
// 描述驱动程序的一些信息，不是必须的
MODULE_VERSION("0.0.1");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SeanXP#iseanxp+code@gmail.com");
MODULE_DESCRIPTION("Tiny6410 Button Driver");
// }}}
