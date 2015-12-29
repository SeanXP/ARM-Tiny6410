# Linux Module
obj-m := char_dev.o
# 当前路径
CURRENT_PATH:=$(shell pwd)
# 交叉编译的linux kernel
LINUX_PATH:=/home/hit413/Code/linux-kernel/linux-2.6.38

TARGET:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_PATH) M=$(CURRENT_PATH) clean



main:	main.c
	gcc	main.c -o main
	@echo "-----------------------读取字符设备的默认值-----------------------"
	sudo ./main read
	@echo "-----------------------给字符设备写入字符串-----------------------"
	sudo ./main write "write what I want to write..."
	@echo "-----------------------再次读取字符设备-----------------------"
	sudo ./main read
	@echo "-----------------------查看内核输出消息-----------------------"
	dmesg | tail
insmod:
	@echo "-----------------------加载字符驱动模块-----------------------"
	sudo insmod char_dev.ko
	dmesg | tail
	@echo "-----------------------创建字符设备节点-----------------------"
	sudo mknod /dev/char_dev c 250 0
	ls -l /dev/char_dev
rmmod:
	@echo "-----------------------卸载字符驱动模块-----------------------"
	sudo rmmod char_dev
	dmesg | tail
	@echo "-----------------------删除字符设备节点-----------------------"
	sudo rm /dev/char_dev
