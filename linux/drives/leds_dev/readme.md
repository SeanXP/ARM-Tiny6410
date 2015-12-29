Linux Module - LED Driver
====

裸机程序: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/no-os/led/myled]

使用:

1. make(前提是有编译好的linux kernel, 并在Makefile中指定好路径)
2. 拷贝生成的leds_dev.ko文件到ARM
3. `insmod leds_dev.ko`
4. 交叉编译测试程序leds\_dev\_test.c
5. 拷贝至ARM
6. `./leds_dev_test 0 1`, 点亮所有LED灯

其他用法看驱动程序的ioctl()功能;

----

提供了两种不同的leds驱动编写方法
  
*  leds_dev.c    
	字符混杂设备miscdevice驱动编写    
	1. 自动创建设备节点，卸载后自动删除
	2. 代码中只需要调用`misc_register()`即可完成注册。
	3. 代码中只需要调用`misc_deregister()`即可完成卸载;

	
*  char_leds.c    
	标准字符设备驱动编写
	1. 手动创建设备节点，手动删除
	2. 代码中，使用`alloc_chrdev_region()`分配主次设备号，使用`cdev_init()`和`cdev_add()`来注册和添加字符设备
	3. 需要`cdev_del()`注销字符设备对象，需要`unregister_chrdev_region()`释放已分配的设备号。
	
	
----

##Linux驱动程序分类

对于linux的驱动程序来说，主要分为三种：   

1. miscdevice
2. platform_device
3. platform_driver

####混杂设备miscdevice

**在Linux驱动中把无法归类的五花八门的设备定义为混杂设备(用miscdevice结构体表述)。**    
Linux/内核所提供的miscdevice有很强的包容性。如NVRAM，看门狗，DS1286等实时时钟，字符LCD,AMD 768随机数发生器。   
**miscdevice共享一个主设备号MISC\_MAJOR(即10)，但次设备号不同。**

	[root@FriendlyARM/root]# ls -l /dev/ledsdev
	crw-rw----    1 root     root       10,  53 May  4 04:19 /dev/ledsdev


所有的miscdevice设备形成了一个链表，对设备访问时内核根据次设备号查找对应的miscdevice设备，然后调用其file_operations结构中注册的文件操作接口进行操作。   
miscdevice的API实现在drivers/char/misc.c中。 

	struct miscdevice  
	{
		int minor;
		const char *name;
		const struct file_operations *fops;
		struct list_head list;
		struct device *parent;
		struct device *this_device;
		const char *nodename;
		mode_t mode;
	};

minor是这个混杂设备的次设备号，若由系统自动配置，则可以设置为`MISC_DYNANIC_MINOR`，name是设备名；

	static struct miscdevice myfirst_led_dev = {
		.minor			=	 MISC_DYNAMIC_MINOR,
		.name			=	 DEVICE_NAME,
		.fops			=	 &S3C6410_LEDS_FOPS,
	};
	

----

1. 应用程序打开设备DEVICE_NAME(open())
2. 应用程序对DEVICE_NAME进行系统调用(read(), write(), ioctl()...)，会触发Linux内核调用DEVICE_NAME对应的驱动程序的file\_operations结构体中的对应函数。例如应用程序里面的ioctl()会触发linux内核执行对应驱动程序的xxx\_ioctl()函数。
3. 用户程序运行在用户空间，驱动程序运行在内核空间。对于一些外设，例如LED灯、Button按键等外设，用户是无法自己编程配置的，只能通过系统调用，触发对应的驱动程序函数，由对应的驱动函数在内核空间对外设寄存器进行配置。
4. file\_operations结构体就是给告诉linux内核该调用哪个函数。

.

分析leds\_dev.c:

    static struct file_operations S3C6410_LEDS_FOPS =
    {
                                            // #define THIS_MODULE (&__this_module); __this_module在编译模块时自动创建;
        .owner  =   THIS_MODULE,            // 这是一个宏，推向编译模块时自动创建的__this_module变量,
        // 驱动函数映射
        .unlocked_ioctl  =  s3c6410_leds_ioctl,     // 对于LED, 实现一个ioctl函数就足够;
    };
    
用户程序内系统调用ioctl(fd, cmd, arg)函数，linux会查找fd对应的设备DEVICE\_NAME对应的驱动设备的file\_operation结构体变量，然后执行其.ioctl函数，同时将参数传递。

例如，要点亮LED灯，在用户程序中，可以这样进行系统调用：

	ioctl(fd, 2, 0);

最后linux会执行对应的驱动函数调用:

	s3c6410_leds_ioctl(file, 2, 0);
	
同样，驱动函数的返回值，会返回给对应的系统调用。

----

对于混杂设备miscdevice，每个混杂设备都有一个struct miscdevice结构体的变量。

    static struct miscdevice misc  =
    {
        .minor = MISC_DYNAMIC_MINOR,        //动态分配次设备号
        .name = DEVICE_NAME,                //设备名称
        .fops = &S3C6410_LEDS_FOPS,
    }; //}}}
    
这里，填充了设备的设备号（动态分配），设备名称，设备的file\_operations；

这样，当用户程序打开一个DEVICE\_NAME, 并对其进行系统调用(例如ioctl)，linux通过查看设备的misc device结构体变量，就能找到匹配的name和设备号.minor，就可以执行对应.fops.ioctl函数。

对于混杂设备，已经在misc device结构体变量中实现了联系：

	1. 设备名称（用于linux根据名称查找驱动程序）
	2. 设备号
	3. 设备file_operation（用于执行对应的驱动函数）

所以，只需要的module\_init函数中，执行：

	ret = misc_register(&misc);
	
这一句，就是利用已初始化好的misc device结构体变量，向linux注册一个新的misc设备，同时会自动创建对应的设备节点 /dev/xxx；

----

对于字符设备，如何实现上面的3个联系呢？（设备名称、设备号、fileoperations)

首先每个字符设备也有一个结构体变量，存储一些信息。

    strut cdev{
        struct kobject kobj;                //内嵌的内核对象
        struct module *owner;               //指向内核模块对象的指针
        const struct file_operations *ops;  //应用程序调用操作系统接口呼叫到设备驱动程序中的相应操作
        struct list_head list;              //链表
        dev_t dev;                          //字符设备的设备号
        unsigned int count;                 //次设备号的个数
    };
    
但是并没有向misc device那样一条语句就自动注册，而是分为以下几步（module\_init函数中）：

	cdev_init(&chr_dev, &chr_ops);  //调用函数初始化字符设备对象
	
将配置号的file\_operations变量写入cdev.ops;

	ret = alloc_chrdev_region(&ndev, 0, 1, DEVICE_NAME);    //自动分配设备号
	
得到系统分配的设备号，例如为(250, 0);

	ret = cdev_add(&chr_dev, ndev, 1);      //将对应的字符设备对象chr_dev注册进系统
	
将刚分配的设备号写入cdev.dev;

可以看到，对于字符设备的init，只有将设备的fileoperations与设备号绑定到对应的驱动程序对应的cdev（通过在驱动程序中配置cdev.owner = THIS_MODULE实现cdev变量与驱动程序的绑定，否则配置cdev变量是没有意义的）

那么目前，用户程序对DEVICE-NAME的配置，是无法映射到目前的驱动程序的。

对于这样的字符设备，需要手动建立设备节点/dev/xxx;

首先动态加载模块:
	
	# insmod char_dev.ko

其中动态加载模块需要有root权限，因为驱动程序要在内核空间运行，具有修改外设寄存器的权限，所以必须要root权限，保证驱动程序值得信任。

然后查看内核输出消息，确定驱动程序分配到的设备号。

	# dmesg | tail
	
最后创建设备节点

	# mknod /dev/xxx c 250 0
	
这样就实现了DEVICE-NAME与设备号的绑定。
而设备号又是和驱动程序绑定的，这样就实现了DEVICE-NAME与驱动程序的绑定，这样在用户程序对DEVICE-NAME的系统调用，就能触发内核空间内对应的驱动含糊。

