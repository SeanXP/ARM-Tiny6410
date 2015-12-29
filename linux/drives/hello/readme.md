linux module - helloworld.ko
====


### 编译配置

一个简单的helloworld模块, 任何linux内核都能加载成功的模块。
用来学习linux Module的基本写法, Makefile决定了模块如何编译。

Makefile中，`LINUX_PATH:=/home/hit413/Code/linux-kernel/linux-2.6.38`指定了Linux内核源码路径。

如果是编译本机的模块：

	VERSION_NUM:=$(shell uname -r)
	LINUX_PATH:=/usr/src/linux-headers-$(VERSION_NUM)

例如本机的`uname -r`为3.13.0-66-generic，则Linux Kernel源码路径为/usr/src/linux-headers-3.13.0-66-generic。    
后缀'-generic'，内部的文件一般都是非后缀、相同名字的目录的符号链接。
	
编译非主机的Linux Kernel模块，则指定对应内核路径即可：

	LINUX_PATH:=/home/hit413/Code/linux-kernel/linux-2.6.38
	
----

### 使用(ARM下)

	[root@FriendlyARM/]# insmod helloworld.ko
	helloworld: module license 'DUAL BSD/GPL' taints kernel.
	Disabling lock debugging due to kernel taint
	Hello World enter the hello module

	[root@FriendlyARM/]# lsmod
	helloworld 778 0 - Live 0xbf000000 (P)

	[root@FriendlyARM/]# rmmod helloworld
	rmmod: chdir(/lib/modules): No such file or directory

注意，加载时是加载模块，因此**insmod命令中指定的是模块文件名(helloworld.ko);**            
卸载时是卸载模块，**rmmod命令中指定的是模块名(helloworld).**   

卸载内核模块时报错，是因为现在的内核模块在插入卸载时都会要跳转到/lib/modules/内核版本号/ 这个目录里。    
所以只要建立这个目录并且把要使用的模块.ko文件复制到这个目录就行了。

	[root@FriendlyARM/]# mkdir -p /lib/modules/$(uname -r)


卸载模块时，遇到问题：

	[root@FriendlyARM/]# rmmod helloworld
	Hello World exit the hello module
	rmmod: module 'helloworld' not found

卸载掉模块，虽然成功，但是每次都多一句：

	rmmod: module '模块名' not found    

经测试得知rmmod: module '模块名' not found 来自于busybox的问题 , 并不是来自kernel。
      
解决方案如下：    

使用另一份源码生成rmmod命令,就可以没有任何提示的卸载ko模块了   
