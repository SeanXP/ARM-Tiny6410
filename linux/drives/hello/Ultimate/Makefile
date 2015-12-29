# ifneq (<arg1>;, <arg2>;), 如果不相同, 则为真;
# 判断KERNELRELEASE是否为空, 为空则未定义KERNELRELEASE, 表示从命令行调用make, 则跳过此if分支;
# 不未空, 表示在内核构造系统目录下调用make, 则添加obj-m
ifneq ($(KERNELRELEASE),)
	obj-m := hello.o
else
	KERNELDIR 	?= /lib/modules/$(shell uname -r)/build
	PWD			:= $(shell pwd)	

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean


# 这种Makefile的写法，可以在内核构造目录外单独编译，也可以放在内核目录下编译;

# 内核构造目录外，单独编译: (在此模块目录下, 通过命令行命令: "$ make"进行编译)
# 1. KERNELRELEASE为空, ifneq为假, 执行else-endif分支(指定内核构造目录 & 模块源码目录);
# 2. make时没有指定目标, 则默认指定第一个非'.'目标, 即default;
# 3. "$(MAKE) -C $(KERNELDIR) M=$(PWD) modules"会进入KERNELDIR目录执行"make modules";
# 4. 在KERNELDIR的Makefile中的modules目标, 会再次读取此模块目录下的Makefile, 届时仅执行ifneq分支, 即设置模块目标为hello.o;
# 5. 在KERNELDIR的Makefile中的modules目标, 根据配置好的obj-m编译出模块, 即hello.ko;

# 内核构造目录内, 和其他模块共同编译 (即在内核顶级目录, 输入"$ make", 按照顺序进行make module)
# 内核顶级Makefile中配置KERNELRELEASE变量, 进入ifneq分支
# 该分支仅一行代码, 即设置obj-m为hello.o
# 之后会在内核顶级目录的Makefile中的make modules时自动编译此模块;


#----------old-----------------
# 构造模块(hello.o -> hello.ko)
#obj-m 		:= hello.o
# 构造模块所需文件 hello.o
#module-objs	:= hello.o
#CURRENT_PATH:=$(shell pwd)
#VERSION_NUM:=$(shell uname -r)
#LINUX_PATH:=/usr/src/linux-headers-$(VERSION_NUM)

#all:
#	make -C $(LINUX_PATH) M=$(CURRENT_PATH) modules
#clean:
#	make -C $(LINUX_PATH) M=$(CURRENT_PATH) clean

