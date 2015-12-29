# Linux Module
OBJECT=char_udev_leds
obj-m := $(OBJECT).o
# 当前路径
CURRENT_PATH:=$(shell pwd)
# 交叉编译的linux kernel
LINUX_PATH:=/home/hit413/Code/linux-kernel/linux-2.6.38

TARGET:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) clean
send:
	make
	cp $(OBJECT).ko /var/nfsroot/rootfs/root/ 
	make clean
