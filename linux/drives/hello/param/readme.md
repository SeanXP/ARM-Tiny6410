带参数的模块
====

Usage:

	$ make
	$ make insmod
	$ dmesg | tail -n 20
	$ make rmmod

-----

驱动需要知道的几个参数因不同的系统而不同. 
从使用的设备号到驱动应当任何操作的几个方面.

例如, SCSI 适配器的驱动常常有选项控制标记命令队列的使用, 
IDE 驱动允许用户控制 DMA 操作. 
如果你的驱动控制老的硬件, 
还需要被明确告知哪里去找硬件的 I/O 端口或者 I/O 内存地址. 
内核通过在加载驱动的模块时指定可变参数的值, 支持这些要求.

这些参数的值可由 insmod 或者 modprobe 在加载时指定; 
后者也可以从它的配置文件(/etc/modprobe.conf)读取参数的值. 

这些命令在命令行里接受几类规格的值. 

	insmod hellop howmany=10 whom="Mom" 

一旦以那样的方式加载, hellop 会说 "hello, Mom" 10 次.

但是, 在 insmod 可以修改模块参数前, 模块必须使它们可用. 

参数用 moudle_param 宏定义来声明, 它定义在 moduleparam.h. 
module_param 使用了 3 个参数: 变量名, 它的类型, 以及一个权限掩码用来做一个辅助的 sysfs 入口. 

这个宏定义应当放在任何函数之外, 典型地是出现在源文件的前面. 
因此 hellop 将声明它的参数, 并如下使得对 insmod 可用:

static char *whom = "world";
static int howmany = 1;
module_param(howmany, int, S_IRUGO);
module_param(whom, charp, S_IRUGO);

支持的默认参数类型: bool, invbool, charp, int, long, short, uint, ulong, ushort.

数组参数, 用逗号间隔的列表提供的值, 模块加载者也支持. 声明一个数组参数, 使用:
`module_param_array(name,type,num,perm);`
这里 name 是你的数组的名子(也是参数名), type 是数组元素的类型, num 是一个整型变量, perm 是通常的权限值. 如果数组参数在加载时设置, num 被设置成提供的数的个数. 模块加载者拒绝比数组能放下的多的值.

