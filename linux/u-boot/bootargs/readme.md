内核启动参数(bootargs)
====

U-boot的环境变量值得注意的有两个： bootcmd 和bootargs。

* bootcmd, 自动启动时默认执行的一些命令; 
* bootargs, 传递给内核的相关参数, 有着固定的格式;

----

### 示例(NFS挂载文件系统)

	setenv bootargs "root=/dev/nfs console=ttySAC0,115200 init=/linuxrc nfsroot=10.42.1.100:/var/nfsroot/rootfs ip=10.42.1.70:10.42.1.100:10.42.1.254:255.255.255.0:name:eth0:off"

1. `root=/dev/nfs`，采用NFS设备启动文件系统
2. `console=ttySAC0,115200`，串口ttySAC0作为控制台，波特率为115200
3. `init=/linuxrc`，设置内核执行的初始化进程为/linuxrc。如果该项没有设置，内核会按顺序尝试/etc/init，/bin/init，/sbin/init，/bin/sh，如果所有的都没找到，内核会抛出 kernel panic的错误。
4. `nfsroot=10.42.1.100:/var/nfsroot/rootfs`，设置NFS目录。
5. `ip=10.42.1.70:10.42.1.100:10.42.1.254:255.255.255.0:name:eth0:off`
	1. 命令格式为`ip=IP0:IP1:IP2:IP3:name:eth0:off`
	2. 第一项(IP0)为目标板的IP地址，这里为10.42.1.70;
	3. 第二项(IP1)为rootserver，这里为10.42.1.100;
	4. 第三项(IP2)为网关，这里为10.42.1.254;
	5. 第四项(IP3)为子网掩码, 这里为255.255.255.0;
	6. 第五项(name)为开发板名称，任意;
	7. 第六项(eth0)为网卡设备名称
	8. off

----

###参数
1. root, 指定rootfs的位置。     
	* `root=/dev/nfs`, 采用NFS设备启动文件系统;
	* `root=/dev/mtdblock2 rw`，使用块设备mtdblock2启动文件系统; 即文件系统已事先存放在mtdblock2中;`rw`和`ro`指定是读写还是只读方式;
	* `root=/dev/ram0 rw`, 使用/dev/ram0设备启动文件系统; RamDisk就是将内存（Ram）的一部分当做硬盘（Disk）来使用。RamDisk有固定的大小，可以像正常硬盘分区那样去使用。 
	* `root=ubi0:rootfs`，使用ubi设备启动文件系统;
	
2. rootfstype, 指定文件系统类型, 与root配套使用。
	* `rootfstype=ubifs`
	* `rootfstype=ext3`
	* `rootfstype=jffs2`
	* ...
3. console, 控制台串口。
	* `console=tty`, 使用虚拟串口终端设备;
	* `console=ttyS[,options]`，使用特定的串口，options可以是这样的形式bbbbpnx，这里bbbb是指串口的波特率，p是奇偶位（从来没有看过使用过），n是指的bits。如`console=ttySAC0,115200n8`，表示控制台串口为ttySAC0, 波特率为115200，8位数据位。
	* `console=ttySAC[,options]`
4. ramdisk_size，配套ramdisk使用，指定/dev/ramx的大小。
	* `ramdisk_size=4M`
5. initrd, noinitrd
	* `initrd=r_addr,size`，如`initrd=0x32000000,0xa00000`。使用ramdisk启动系统，r_addr表示initrd在内存中的位置，size表示initrd的大小。
	* `noinitrd`，声明没有使用ramdisk启动系统;
6. init，内核启动以后，运行的第一个脚本。
	* `init=/linuxrc`
	* `init=/etc/preinit`
7. ip，指定系统启动之后网卡的ip地址   
	如果使用基于nfs的文件系统，那么必须要设置IP参数，其他的情况可以不设置。
	* `ip=off`，不设置ip
	* `ip=ip_addr`，例如`ip=10.42.1.70`
	* `ip=ip_addr:server_ip_addr:gateway:netmask::which_netcard:off`
8. mtdparts，mtd分区配置    
	
	要想这个参数起作用，内核中的mtd驱动必须要支持。    
	mtdparts的格式如下：
		
		mtdparts=<mtddef>[;<mtddef]
		<mtddef>  := <mtd-id>:<partdef>[,<partdef>]
		<partdef> := <size>[@offset][<name>][ro]
		<mtd-id>  := unique id used in mapping driver/device
		<size>    := standard linux memsize OR "-" to denote all remaining space
		<name>    := (NAME)
	* 参考格式: `mtdparts=mtd-id:<size1>@<offset1>(<name1>),<size2>@<offset2>(<name2>)`
	* mtd-id必须要与当前平台的flash的mtd-id一致，在bootargs参数列表中可以指定当前flash的mtd-id，如默认指定`U-Boot# setenv mtdids nand0=nand_flash`，前面的nand0则表示第一个flash。
	* `mtdparts=nand_flash:128k(u-boot)ro,64k(u-boot envs),3m(kernel),30m(root.jffs2),30m(root.yaffs)`

	
-----

###常用配置

1. 从ramdisk启动文件系统：
	
		setenv bootargs 'root=/dev/ram0 initrd=0x32000000,0xa00000 console=ttySAC0,115200n8 mem=64M init=/linuxrc'
	
	* 从/dev/ram0中启动文件系统
	* /dev/ram0在内存中的起始地址为0x32000000，大小为0xa00000
	* 控制台串口为ttySAC0，波特率115200，数据位8-bit
	* 内存64M
	* 初始运行脚本/linuxrc

2. 从flash的jffs2镜像启动文件系统
	
		setenv bootargs 'root=/dev/mtdblock2 rw noinitrd rootfstype=jffs2 console=ttySAC0,115200n8 mem=64M init=/linuxrc'
	
	* jffs2文件系统镜像存放在flash的mtdblock2分区中
	* 配置文件系统格式为jffs2

3. 基于NFS启动文件系统
	
		setenv bootargs 'root=/dev/nfs rw noinitrd console=ttySAC0,115200n8 mem=256M init=/linuxrc nfsroot=10.42.1.100:/var/nfsroot/rootfs ip=10.42.1.70:10.42.1.100:10.42.1.254:255.255.255.0:tiny6410:eth0:off'
		
	