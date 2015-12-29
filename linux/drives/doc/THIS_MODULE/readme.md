THIS_MODULE
====

结构体`struct module`在内核中代表一个内核模块，通过`insmod`(实际执行init_module系统调用)把自己编写的内核模块插入内核时，模块便与一个`struct module`结构体相关联，并成为内核的一部分。

###struct module

`<linux-2.6.38/include/linux/module.h>`

    struct module
    {
        enum module_state state;

        /* Member of list of modules */
        struct list_head list;

        /* Unique handle for this module */
        char name[MODULE_NAME_LEN];

        /* Sysfs stuff. */
        struct module_kobject mkobj;
        struct module_attribute *modinfo_attrs;
        const char *version;
        const char *srcversion;
        struct kobject *holders_dir;

        /* Exported symbols */
        const struct kernel_symbol *syms;
        const unsigned long *crcs;
        unsigned int num_syms;

        /* Kernel parameters. */
        struct kernel_param *kp;
        unsigned int num_kp;

        /* GPL-only exported symbols. */
        unsigned int num_gpl_syms;
        const struct kernel_symbol *gpl_syms;
        const unsigned long *gpl_crcs;
        
        ....
        ....
        ....
        
----


加载一个内核模块，一般会使用工具insmod，该工具实际上调用了系统调用init\_module，在该系统调用函数中，首先调用 load\_module，把用户空间传入的整个内核模块文件创建成一个内核模块，返回一个struct module结构体。内核中便以这个结构体代表这个内核模块。

* enum module_state state;    
	state是模块当前的状态。   
	它是一个枚举型变量，可取的值为：

	* MODULE_STATE_LIVE，模块当前正常使用中 （存活状态）
	* MODULE_STATE_COMING，模块当前正在被加载
	* MODULE_STATE_GOING ，模块当前正在被卸载
	 
	load\_module函数中完成模块的部分创建工作后，把状态置为 MODULE\_STATE\_COMING；    
	sys\_init\_module函数中完成模块的全部初始化工作后（包括把模块加入全局的模块列表，调用模块本 身的初始化函数)，把模块状态置为MODULE\_STATE\_LIVE。    
	最后使用rmmod工具卸载模块时，会调用系统调用 delete\_module，会把模块的状态置为MODULE\_STATE\_GOING。这是模块内部维护的一个状态。    
	
* struct list_head list;    
	list是作为一个列表的成员，所有的内核模块都被维护在一个全局链表中，链表头是一个全局变量struct module *modules。任何一个新创建的模块，都会被加入到这个链表的头部，通过modules->next即可引用到。
* char name[MODULE_NAME_LEN];   
    name是模块的名字，一般会拿模块文件的文件名作为模块名。它是这个模块的一个标识。
    
----

###宏`THIS_MODULE`

	#define THIS_MODULE (&__this_module)
	
`__this_module`
是一个struct module变量，代表当前模块，跟current有几分相似。可以通过`THIS_MODULE`宏来引用模块的struct module结构。