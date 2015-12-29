Tiny6410 - Arm Linux
=====


#烧录
参考: 03- Tiny6410刷机指南.pdf

假设拿到的Tiny6410开发板没有提前下载任何程序，包括Bootloader.

###Bootloader - Superboot

Superboot是FriendlyARM公司提供的Bootloader(非开源)，提供USB下载功能。   
只要烧写了Superboot, 就可以通过USB下载内核、文件系统到板子的Flash中。

因为Nand Flash现在还没有内容，现在只有通过SD卡启动。    

完全空白的 SD 卡是不能直接启动 6410 开发板的,必须先在 PC 上使用特殊的烧写软 件把 BIOS(也可以称为 bootloader)写入 SD 卡才可以,并且写入的这个 BIOS 是无法在电脑上直接看到的。

一般市场上买到的 SD 卡为全盘 FAT32 格式,如果卡中存放了很多数据,强制烧写就可能会不知不觉中破坏这些数据;   
基于此原因考虑, FriendlyARM在 Vista/Winows7 中,先把 SD 卡自动分割为普通的 FAT32 格式区(自动命名卷标为"FriendlyARM")和无格式区(占 130M) 两部分,烧写软件将会依据卷标名称作为标志,把 bootloader 烧写到无格式区,这样就不会 破坏普通 FAT32 格式区中的数据了。

Windows 7，使用FriendlyARM提供的SD卡烧录工具**SD-Flasher.exe**:    

1. SD卡连接Window 7（提前备份SD卡数据，下面的操作会格式化SD卡）
2. 以管理员身份运行SD-Flasher.exe
3. 点击"Scan"按钮，选择找到的SD卡
4. 点击"ReLayout!"按钮，对SD卡进行分区
	* FAT32
	* 无格式区, SD卡最后的130M空间;    
	
	例如一个8G的SDHC卡, 7.4 GiB, 7948206080 bytes, 15523840 sectors（2048-15523839，前2048个扇区被保留）:
	
	* FAT32, (第2048扇区~15259647扇区），共15257600扇区。
	* 无格式区, (第15259648扇区 ~ 15523840扇区)，共有264193扇区（约129M）
	
	除了FAT32分区以外，其他扇区总数共266240个扇区。     
	而前2048个扇区，为FAT32的保留扇区。作为“第一分区”，存放主启动记录(MBR)和分区表，占512字节，其中最后两个字节为信号字"0x55","0xaa"：
	`% sudo dd if=/dev/sdc bs=1 skip=510 count=2 | od -t x1c`
5. 分区完成后，重新"Scan"，此时SD卡的"Available"栏目信息变为"Yes"。
6. 选择Image File，这里选择Bootloader文件(superboot-20111114.bin)
7. 点击"Fuse"按钮，烧录Bootloader到SD卡
8. 将SD卡插入Tiny6410开发板卡槽，选择SDBOOT启动模式，连接串口，上电，会看到串口输出消息：

        ##### FriendlyARM Superboot for 6410 #####
        [f] Format the nand flash
        [p] Download superboot
        [v] Download uboot.bin
        [k] Download Linux/Android kernel
        [y] Download root yaffs2 image
        [u] Download root ubifs image
        [a] Download Absolute User Application
        [n] Download Nboot.nb0 for WinCE
        [l] Download WinCE bootlogo
        [w] Download WinCE NK.bin
        [b] Boot the system
        [s] Set the boot parameter of Linux
        [d] Download and Run an Absolute User Application
        [i] Version: 1144, RAM 1024 MiB, NAND(SLC) 1GiB
        Please enter your Selection:
 

要使用Superboot的USB下载功能，需要：

1. Windows XP (Window 7 不兼容）
2. USB驱动(FriendlyARM USB Download Driver Setup_20090421)
3. DNW下载软件

下载步骤（通过SD卡的Superboot下载Superboot/Kernel/File System到Nand Flash)：

1. 连接串口至电脑用于查看串口输出，连接USB线至电脑用于传输数据。
2. 启动ARM开发板，进入Superboot。   
		
	* Nand Flash下的Superboot, 会自动引导Linux内核。因此需要按住按钮再上电。
	* SD卡的Superboot, 可以直接上电。

3. 选择[p]选项，烧录Superboot.bin到Nand Flash; 以后就可以脱离SD卡。
4. 选择[k]选项，烧录zImage(Linux内核文件)到Nand Flash;
5. 选择[y]/[u]选项，烧录不同格式的文件系统到Nand Flash;
6. 选择[b]选项，启动Nand Flash内的Linux内核。

另外，还可以通过[d]选项，直接下载程序(例如之前交叉编译好的leds.bin, key.bin等）到内存起始地址，并开始运行程序。


###Bootloader - U-boot

使用Tekkaman Ninja移植的[U-boot-2011.06](https://github.com/tekkamanninja/u-boot-2011.06-for-MINI6410).

SD卡烧录U-Boot.bin:

[no-os/sd-no-os/u-boot](https://github.com/SeanXP/ARM-Tiny6410/tree/master/no-os/sd-no-os/u-boot) 

**SDBOOT启动U-Boot，通过fatload烧录U-Boot到NAND Flash:**

1. 判断mmc设备号
	
		MINI6410 # mmc dev
		mmc0 is current device
2. 查看SD卡的文件

		MINI6410 # fatls mmc 0
		270336   superboot-20111114.bin
		255576   u-boot.bin
		4096   u-boot-spl-16k.bin
		259672   u-boot-nand.bin

		4 file(s), 0 dir(s)
	需要事先拷贝u-boot-nand.bin到SD卡。
3. 读取u-boot-nand.bin到SDRAM

		MINI6410 # fatload mmc 0 50008000 u-boot-nand.bin
		reading u-boot-nand.bin

		259672 bytes read
4. 将内存中的u-boot-nand.bin写入到Nand Flash的Bootloader区域.
	
	这里的Bootloader为0 ~ 0x80000(512K)
	
		MINI6410 # nand erase 0 0x80000
		NAND erase: device 0 offset 0x0, size 0x80000
		Erasing at 0x60000 -- 100% complete.
		OK
		
		MINI6410 # nand write 50008000 0 80000
		NAND write: device 0 offset 0x0, size 0x80000
		524288 bytes written: OK
 
5. 切换为NAND启动，上电，会看到Nand Flash上的U-Boot启动。

**SDBOOT启动U-Boot，通过NFS烧录U-Boot到NAND Flash:**

1. 搭建NFS服务器，目录:  10.42.1.100:/var/nfsroot/arm/
2. SDBOOT启动U-Boot

		MINI6410 # nfs 50008000 10.42.1.100:/var/nfsroot/arm/u-boot-nand.bin
		dm9000 i/o: 0x18000300, id: 0x90000a46
		DM9000: running in 16 bit mode
		MAC: 08:08:10:12:10:27
		operating at 100M full duplex mode
		Using dm9000 device
		File transfer via NFS from server 10.42.1.100; our IP address is 10.42.1.70
		Filename '/var/nfsroot/arm/u-boot-nand.bin'.
		Load address: 0x50008000
		Loading: 	###################################################
		doen
		Bytes transferred = 259672 (3f658 hex)
3. 烧写进Nand Flash

		MINI6410 # nand erase 0x0 0x80000
		NAND erase: device 0 offset 0x0, size 0x80000
		Erasing at 0x60000 -- 100% complete.
		OK
		
		MINI6410 # nand write 50008000 0 0x80000
		NAND write: device 0 offset 0x0, size 0x80000
		524288 bytes written: OK
		
**SDBOOT启动U-Boot，通过NFS烧录linux kernel到NAND Flash:**   

注意：**不要用U-boot引导FriendlyARM官方给的zImage，不同bootloader对内核的引导略有不同，最好利用U-Boot的mkimage重新生成uImage.** 使用U-boot引导zImage，会导致"Error: unrecognized/unsupported machine ID"的问题。

Linux Kernel: 使用FriendlyARM提供的linux-2.6.38-20140106.tgz。

1. 交叉编译linux内核，得到vmlinux
2. 使用mkimage(u-boot-2011.06-for-MINI6410/tools/mkimage), 生成U-Boot格式的映像文件uImage.
	
		arm-linux-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
		gzip -9 linux.bin
		./mkimage -A arm -O linux -T kernel -C gzip -a 0x50008000 -e 0x50008000 -n "Linux Kernel Image" -d linux.bin.gz uImage
		
	
	输出:
	
		Image Name:   Linux Kernel Image
		Created:      Fri Nov  6 11:11:04 2015
		Image Type:   ARM Linux Kernel Image (gzip compressed)
		Data Size:    3742549 Bytes = 3654.83 kB = 3.57 MB
		Load Address: 50008000
		Entry Point:  50008000
		
	对于ARM linux内核映象用法：

	* -A arm , 架构是arm
	* -O linux , 操作系统是linux
	* -T kernel , 类型是kernel
	* -C none/bzip/gzip , 压缩类型
	* -a 50008000 , image的载入地址(hex)，通常为0xX00008000
	* -e 500080XX , 内核的入口地址(hex)，XX为0x40或者0x00
	* -n linux-XXX , image的名字，任意
	* -d nameXXX , 无头信息的image文件名，你的源内核文件
	* uImageXXX , 加了头信息之后的image文件名，任意取

	-a和-e后面跟的分别是image的载入地址(Load Address)和内核的入口地址(Enter Point)，两者可以一样，也可以不一样，依情况而定：   
  
	* 当-a后面指定的地址和bootm xxxx后面的地址一样时(此时u-boot不需要搬运映像文件uImage)，则-e后面的地址必须要比-a后面的地址多0x40，也就是映像头的大小64个字节。
	* 当-a后面指定的地址和bootm xxxx后面的地址不一样时（u-boot会将bootm xxxx地址处的映像搬运到-a指定的地址处），此时，-e和-a必须要一样，因为映像头并没有搬运过去，载入地址就应该是内核的入口地址。需要注意的是，因为uboot要重新搬运内核映像，所以要注意bootm xxxx的地址和-a之间的地址不要导致复制时的覆盖。     
	例如：   
	Load Address: 0x0000000
	u-boot: bootm 0x00010000
	地址不同，需要搬运uImage, 不搬运头部64字节。但是uImage在搬运过程中，会覆盖到旧的uImage, 导致错误:
	
	
			Uncompressing Kernel Image ... Error: inflate() returned −3
			GUNZIP ERROR − must RESET board to recover

		

3. 通过NFS烧录(U-Boot Command Line):

		nfs 0x50008000 10.42.1.100:/var/nfsroot/arm/uImage
		nand erase 0x80000 0x500000
		nand write 0x50008000 0x80000 0x500000
	
4. 配置U-Boot参数bootcmd

		MINI6410 # setenv bootcmd "nand read 0x51008000 0x80000 0x500000; bootm 0x51008000"
		MINI6410 # saveenv
	从Nand Flash的0x80000处，读取5M数据，存放到SDRAM的0x51008000处，然后调用bootm函数。0x51008000处存放着uImage, 其头部描述Load Address为0x50008000，与bootm的参数(0x51008000）不同，则拷贝0x51008000处的uImage到0x50008000, 不拷贝头部。然后进入Entry Point(0x50008000), 内核启动成功。
	
----

###File System - 文件系统

1. 制作文件系统     
	* 参考: 韦东山 - 17章节    
	* 参考: [ 一步一步制作yaffs/yaffs2根文件系统](http://blog.csdn.net/mybelief321/article/details/9995199)
2. NFS挂载     
	制作好根文件系统，先别着急制作成yaffs2格式的映像，别着急下载到Nand Flash，先通过NFS挂载文件系统，测试文件系统是否正常。
	
		setenv bootargs 'root=/dev/nfs rw noinitrd console=ttySAC0,115200n8 mem=256M init=/linuxrc nfsroot=10.42.1.100:/var/nfsroot/rootfs ip=10.42.1.70:10.42.1.100:10.42.1.254:255.255.255.0:tiny6410:eth0:off'
		saveenv
		boot
		
	Linux内核在启动的时候，能接收某些命令行选项或启动时参数。   
	内核启动参数详解：
	
	1. `root=/dev/nfs rw`，采用NFS设备启动文件系统，`rw`可读可写。
	2. `noinitrd`，非ramdisk启动方式。
	2. `console=ttySAC0,115200n8`，串口ttySAC0作为控制台，波特率为115200，8bit数据位。
	3. `mem=256M`，内存为256M.
	3. `init=/linuxrc`，设置内核启动后执行的第一个脚本为/linuxrc。如果该项没有设置，内核会按顺序尝试/etc/init，/bin/init，/sbin/init，/bin/sh，如果所有的都没找到，内核会抛出 kernel panic的错误。
	4. `nfsroot=10.42.1.100:/var/nfsroot/rootfs`，设置文件系统所在的NFS目录。
	5. `ip=10.42.1.70:10.42.1.100:10.42.1.254:255.255.255.0:tiny6410:eth0:off`
		1. 命令格式为`ip=ip_addr:server_ip_addr:gateway:netmask:name:which_netcard:off`
		2. 第一项(ip_addr)为目标板的IP地址，这里为10.42.1.70;
		3. 第二项(server\_ip\_addr)为rootserver，这里为10.42.1.100;
		4. 第三项(gateway)为网关，这里为10.42.1.254;
		5. 第四项(netmask)为子网掩码, 这里为255.255.255.0;
		6. 第五项(name)为开发板名称，任意;
		7. 第六项(which_netcard)为网卡设备名称;
		8. off;

	挂载文件以后以后，可以在内核输出消息处看到：
	
		IP-Config: Complete:
		device=eth0, addr=10.42.1.70, mask=255.255.255.0, gw=10.42.1.254,
		host=tiny6410, domain=, nis-domain=(none),
		bootserver=10.42.1.100, rootserver=10.42.1.100, rootpath=

		Please press Enter to activate this console.


3. 使用mkyaffs2image-128生成yaffs2映像，烧写到板子上进行测试
	
	mkyaffs2image（适合64M）    
	mkyaffs2image-128（适合128M以上）    
	注：由于FriendlyARM提供的mkyaffs2image-128代码有改动，使用该工具制作的yaffs2映像，在引导时可能存在以下问题：
	
		/init: line 103: can't open /r/dev/console: no such file
		Kernel panic - not syncing: Attempted to kill init!
	虽然网上遇到这种问题的解决办法是：
	
	在根文件系统/dev/目录下，创建console节点:
	
		sudo mknod console c 5 1
	但这里的问题并不是这个。
	Google Search : 
	
		site:arm9home.net /r/dev/console
	用自己编译的内核加上友善的yaffs2文件系统，友善的内核加自己的文件系统，自己的内核加自己的文件系统全都会出现上述提示。只有用友善的内核和文件系统才不会报错。 
	
	/r, 这是系统启动时先会mount 命令行参数指定的rootfs到 /r 目录, 准备好后再切换/r 为根目录
如果mount失败, 或烧写的文件系统有问题等等, 那么都会出现这个错误。

	使用SuperBoot-6410与zImage不存在此问题，NFS引导不存在此问题（其他网友存在此问题）。   
	
	
	**暂未解决**

