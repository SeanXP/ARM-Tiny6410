module_init宏分析
====

驱动程序中，经常使用module\_init & module\_exit宏:

	module_init(hello_init);
	module_exit(hello_exit);
	

一个驱动可以作为一个模块动态的加载到内核里，也可以作为内核的一部分静态的编译进内核，
module\_init/module\_exit也就有了两个含义，根据是否定义了MOFDULE宏定义来决定。
	
	
linux-2.6.38/include/linux/init.h:


	#ifndef MODULE
	...
	#define module_init(x)  __initcall(x);
	#define module_exit(x)  __exitcall(x);
	...
	#else /* MODULE */
	...
	/* Each module must use one module_init(). */
	#define module_init(initfn)                 \
		static inline initcall_t __inittest(void)       \
		{ return initfn; }                  \
	    int init_module(void) __attribute__((alias(#initfn)));

	/* This is only required if you want to be unloadable. */
	#define module_exit(exitfn)                 \
		static inline exitcall_t __exittest(void)       \
	    { return exitfn; }                  \
	    void cleanup_module(void) __attribute__((alias(#exitfn)));
	...
	#endif

###动态模块

动态模块是在linux内核启动时不主动加载，而在linux系统启动以后，通过insmod加载模块, 可以通过rmmod卸载模块.

	# insmod hello.ko
	# rmmod hello

**加载模式时, 默认调用init\_module()函数, 
insmod和rmmod只识别默认的init_module & cleanup\_module函数, 
不使用默认命名，则需要使用module_init与module_exit宏, 该宏会检测函数格式并起别名为默认名称。**


	/* Each module must use one module_init(). */
	#define module_init(initfn)                 \
		static inline initcall_t __inittest(void)       \
		{ return initfn; }                  \
	    int init_module(void) __attribute__((alias(#initfn)));

	/* This is only required if you want to be unloadable. */
	#define module_exit(exitfn)                 \
		static inline exitcall_t __exittest(void)       \
	    { return exitfn; }                  \
	    void cleanup_module(void) __attribute__((alias(#exitfn)));

`module_init(hello_init);`会被宏定义扩展为:

1. 验证加载函数的格式

		static inline initcall_t __inittest(void)
		{ return hello_init;}

	这个函数的作用是验证传递的模块加载函数格式是否正确，linux内核规定加载函数的的原型是：

		typedef int (*initcall_t)(void);

	所以写加载函数的时候必须是返回值为int参数为void的函数，这个在内核里要求比较严格，
	所以我们写加载函数的时候必须按照这个约定。

2. 定义别名

		int init_module(void) __attribute__((alias(hello_init)));

	这段代码的作用是给加载函数定义一个别名，别名就是前面提到的init_module，这样insmod就能够执行我们的加载函数。


module\_exit的作用和module\_init一样，同样也是验证函数格式和定义别名。

###静态模块

	/**
	 * module_init() - driver initialization entry point
	 * @x: function to be run at kernel boot time or module insertion
	 *
	 * module_init() will either be called during do_initcalls() (if
	 * builtin) or at module insertion time (if a module).  There can only
	 * be one per module.
	 */
	#define module_init(x)  __initcall(x);	

	#define __initcall(fn) device_initcall(fn)
	
	#define device_initcall(fn)     __define_initcall("6",fn,6)

	/* initcalls are now grouped by functionality into separate
	 * subsections. Ordering inside the subsections is determined
	 * by link order.
	 * For backwards compatibility, initcall() puts the call in
	 * the device init subsection.
	 *
	 * The 'id' arg to __define_initcall() is needed so that multiple initcalls
	 * can point at the same handler without causing duplicate-symbol build errors.
	 */

	#define __define_initcall(level,fn,id) \
		static initcall_t __initcall_##fn##id __used \
		__attribute__((__section__(".initcall" level ".init"))) = fn
	
那么在编译内核时，编译器会将`module_init(hello_init)`转化为如下语句:
	
	static initcall_t __initcall_hello_init6 __used __attribute__((__section__(".initcall6 .init"))) = hello_init;

定义了一个类型为函数指针的变量__initcall_hello_init6, 存放的是hello_init函数地址;     

其中initcall_t是一个函数指针:
	
	/*
	* Used for initialization calls..
	*/
	typedef int (*initcall_t)(void);
	typedef void (*exitcall_t)(void);

\_\_used也是一个宏，在使用GCC3.4之前的编译器被展开成\_\_\_attribute\_\_((unused))来禁止编译器弹出有关函数没有被用到的的警告信息。   

在3.4之后被展开成\_\_attribute\_\_((used))功能一样。     
`__attribute__((__section__(".initcall6 .init")))`将该变量加载到段.initcall6.init上，在链接脚本头文件上可以找到如下语句:

	#define INITCALLS \
		*(.initcallearly.init)\
		VMLINUX_SYMBOL(__early_initcall_end) = .;\
		  *(.initcall0.init)\
		  *(.initcall0s.init)\
		  *(.initcall1.init)\
		  *(.initcall1s.init)\
		  *(.initcall2.init)\
		  *(.initcall2s.init)\
		  *(.initcall3.init)\
		  *(.initcall3s.init)\
		  *(.initcall4.init)\
		  *(.initcall4s.init)\
		  *(.initcall5.init)\
		  *(.initcall5s.init)\
		*(.initcallrootfs.init)\
		  *(.initcall6.init)\
		  *(.initcall6s.init)\
		  *(.initcall7.init)\
		  *(.initcall7s.init)

编译过内核后，生成的vmlinux.lds(linux-2.6.38/arch/arm/kernel/vmlinux.lds)文件中有如下语句:

	__initcall_start = .; 
	*(.initcallearly.init) __early_initcall_end = .; 
	*(.initcall0.init) *(.initcall0s.init) *(.initcall1.init) *(.initcall1s.init) *(.initcall2.init) *(.initcall2s.init) *(.initcall3.init) *(.initcall3s.init) *(.initcall4.init) *(.initcall4s.init) *(.initcall5.init) *(.initcall5s.init) *(.initcallrootfs.init) *(.initcall6.init) *(.initcall6s.init) *(.initcall7.init) *(.initcall7s.init) __initcall_end = .;

说明静态模块的初始函数的地址被放在在initcall6.init段中。
当insmod的时候，内核从initcall6.init段中读取到该地址，然后跳转到该地址去执行，即执行模块的module_init函数;

**在静态编译模块时，module_exit没有意义，静态编译的模块无法卸载**

----
