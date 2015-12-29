参考：
《嵌入式Linux应用开发完全手册》 - 韦东山
（书籍和视频）

参考：[https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux]

----

###学习步骤:

1. 学习U-Boot的移植、交叉编译 、烧写、命令行使用；
2. 学习Linux内核的移植、交叉编译、烧写；
3. 学习文件系统的制作；到了这一步，应该可以让板子起来Linux并正常运行。
4. Linux驱动设备学习；


----

### U-Boot学习 

参考: 《嵌入式Linux应用开发完全手册》 - 韦东山 - 第15章 移植U-Boot

参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/u-boot]    

1. 了解U-boot
	* 什么是U-Boot
	* 为什么要用U-Boot / Bootloader
	* U-Boot的功能
2. U-Boot的交叉编译
	* U-Boot的移植先不要看，先直接用。
	* 下载U-Boot已移植版本，这里使用: [https://github.com/tekkamanninja/u-boot-2011.06-for-MINI6410]    
	
	* 解压，交叉编译:
		
			make distclean
			make mini6410_config
			make
	
		这里提供已交叉编译好的bin文件: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/u-boot/bin]    
	
	* 得到SD卡的U-Boot ( u-boot-2011.06-for-MINI6410/mmc_spl/u-boot-spl-16k.bin, 烧写到SD卡BL1区域。u-boot-2011.06-for-MINI6410/u-boot.bin，烧写到特定区域。
		
		参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/no-os/sd-no-os/u-boot#选择其他u-boot]）
		
	* 得到Nand Flash的U-Boot( u-boot-2011.06-for-MINI6410/u-boot-nand.bin），先烧写SD卡的U-Boot，然后通过U-Boot的命令行，烧写u-boot-nand.bin到Nand Flash.

		参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux#bootloader---u-boot]

3. U-Boot命令学习
	
4. 拓展（不学不影响使用）：

	* 了解U-Boot的编译流程（分析Makefile)       
 
		参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/u-boot/mkconfig]   
		
		参考: [https://github.com/SeanXP/ARM-Tiny6410/blob/master/linux/u-boot/doc/U-boot综述与源码分析.pdf]；
	
	* 了解U-Boot的执行顺序（第一阶段、第二阶段）

		参考: [https://github.com/SeanXP/ARM-Tiny6410/blob/master/linux/u-boot/doc/U-boot综述与源码分析.pdf]；
	
	* 了解U-Boot的移植过程
	
		参考: [https://github.com/SeanXP/ARM-Tiny6410/blob/master/linux/u-boot/doc/mini2440_U-boot使用移植手册.pdf]；


----

### Linux Kernel编译

参考: 《嵌入式Linux应用开发完全手册》 - 韦东山 - 第16章 移植Linux Kernel

1. 了解Linux, Linux Kernel
2. Linux Kernel的交叉编译
	* 这里使用FriendlyARM提供的: Tiny6410-A-14022（光盘）/Linux/linux-2.6.38-20140106.tgz
	* 交叉编译
	
			make distclean
			cp config_linux_mini6410 .config
			make
		得到vmlinux.   
		参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/vmlinux]
	
	* 利用vmlinux制作uImage

			arm-linux-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
			gzip -9 linux.bin
			./mkimage -A arm -O linux -T kernel -C gzip -a 0x50008000 -e 0x50008000 -n "Linux Kernel Image" -d linux.bin.gz uImage
			
		参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/vmlinux] 
		
		mkimage工具为刚才U-Boot交叉编译生成的工具: 
		u-boot-2011.06-for-MINI6410/tools/mkimage
3. 利用U-Boot烧写Linux Kernel

	参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/readme.md]
	
----

###构建File System

参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/file-system]

参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux#file-system---文件系统]

----

###Linux Device Drivers

1. 学习驱动程序的基本写法 & 加载、卸载过程

	参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/drives/hello]
	
2. 字符设备驱动、混杂设备驱动的写法 

	参考: [https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux/drives/leds_dev]

