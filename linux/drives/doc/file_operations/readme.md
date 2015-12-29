file_operations
====


结构体file_operations在头文件`<linux/fs.h>`中定义，用来存储驱动内核模块提供的对设备进行各种操作的函数的指针。   
该结构体的每个域都对应着驱动内核模块用来处理某个被请求的函数的地址。   

###`linux-2.6.38/include/linux/fs.h`

    struct file_operations {
        struct module *owner;
        loff_t (*llseek) (struct file *, loff_t, int);
        ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
        ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
        ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);
        int (*readdir) (struct file *, void *, filldir_t);
        unsigned int (*poll) (struct file *, struct poll_table_struct *);
        long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
        long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
        int (*mmap) (struct file *, struct vm_area_struct *);
        int (*open) (struct inode *, struct file *);
        int (*flush) (struct file *, fl_owner_t id);
        int (*release) (struct inode *, struct file *);
        int (*fsync) (struct file *, int datasync);
        int (*aio_fsync) (struct kiocb *, int datasync);
        int (*fasync) (int, struct file *, int);
        int (*lock) (struct file *, int, struct file_lock *);
        ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
        unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
        int (*check_flags)(int);
        int (*flock) (struct file *, int, struct file_lock *);
        ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
        ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
        int (*setlease)(struct file *, long, struct file_lock **);
        long (*fallocate)(struct file *file, int mode, loff_t offset,
                  loff_t len);
    };
    
    
指向结构体struct file_operations的指针通常命名为fops。

驱动内核模块是不需要实现每个函数，驱动不支持的函数应置为NULL（并且当指定为 NULL 指针时，对于不同的函数，内核的默认行为可能是不同的）。

----

示例:

	static struct file_operations S3C6410_LEDS_FOPS =
    {
                                            // #define THIS_MODULE (&__this_module); __this_module在编译模块时自动创建;
        .owner  =   THIS_MODULE,            // 这是一个宏，推向编译模块时自动创建的__this_module变量,
        // 驱动函数映射
        .open = s3c6410_leds_open,                  // 对于字符设备, open()为初始化;
        //.close = s3c6410_leds_close,
        .release = s3c6410_leds_close,              // 对于字符设备, close()为关闭设备;
        .read = s3c6410_leds_read,                  // 对于字符设备, read()为读取设备相关信息;
        .unlocked_ioctl = s3c6410_leds_ioctl,       // ioctl()为命令控制;
        //  .ioctl = s3c6410_leds_ioctl,        //linux kernel 3.x 版本的内核删除了ioctl函数
        //  取代的是: long (*unlocked_ioctl) (struct file*, unsigned int, unsigned long);
        //            long (*compat_ioctl) (struct file*, unsigned int, unsigned long);
    };



-----

###struct file_operations

* struct module *owner;
	
	第一个成员根本不是一个操作; 它是一个指向拥有这个结构的模块的指针. 这个成员用来在它的操作还在被使用时阻止模块被卸载. 几乎所有时间中, 它被简单初始化为 THIS_MODULE, 一个在 `<linux/module.h>` 中定义的宏.

		#define THIS_MODULE (&__this_module)
	
	`__this_module`
是一个struct module变量，代表当前模块。可以通过`THIS_MODULE`宏来引用模块的struct module结构。

* ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);    

	用来从设备中获取数据。  
	在这个位置的一个空指针导致 read 系统调用以 -EINVAL("Invalid argument") 失败。   
	一个非负返回值代表了成功读取的字节数( 返回值是一个 "signed size" 类型, 常常是目标平台本地的整数类型)。
	
* ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);   

	发送数据给设备。    
	如果 NULL, -EINVAL 返回给调用 write 系统调用的程序。 如果非负, 返回值代表成功写的字节数.

* unsigned int (*poll) (struct file *, struct poll_table_struct *);   

	poll 方法是 3 个系统调用的后端: poll, epoll, 和 select, 都用作查询对一个或多个文件描述符的读或写是否会阻塞。    
	poll 方法应当返回一个位掩码指示是否非阻塞的读或写是可能的, 并且, 可能地, 提供给内核信息用来使调用进程睡眠直到 I/O 变为可能。    
	如果一个驱动的 poll 方法为 NULL, 设备假定为不阻塞地可读可写.

* int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);    

	ioctl 系统调用提供了发出设备特定命令的方法(例如格式化软盘的一个磁道, 这不是读也不是写)。   
	另外, 几个 ioctl 命令被内核识别而不必引用 fops 表. 如果设备不提供 ioctl 方法, 对于任何未事先定义的请求(-ENOTTY, "设备无这样的 ioctl"), 系统调用返回一个错误。    
	
* int (*open) (struct inode *, struct file *);    
	
	尽管这常常是对设备文件进行的第一个操作, 不要求驱动声明一个对应的方法.    
	如果这个项是 NULL, 设备打开一直成功, 但是你的驱动不会得到通知.
	
* int (*flush) (struct file *);
	
	**flush 操作在进程关闭它的设备文件描述符的拷贝时调用**; 它应当执行(并且等待)设备的任何未完成的操作.    
	这个必须不要和用户查询请求的 fsync 操作混淆了. 当前, flush 在很少驱动中使用; SCSI 磁带驱动使用它, 例如, 为确保所有写的数据在设备关闭前写到磁带上. 如果 flush 为 NULL, 内核简单地忽略用户应用程序的请求.

* int (*release) (struct inode *, struct file *);    

	在文件结构被释放时引用这个操作. 如同 open, release 可以为 NULL.   
	
	注意, **release 不是每次进程调用 close 时都被调用.** 无论何时共享一个文件结构(例如, 在一个 fork 或 dup 之后), release 不会调用直到所有的拷贝都关闭了. 如果你需要在任一拷贝关闭时刷新挂着的数据, 你应当实现 flush 方法.
	
	
----





