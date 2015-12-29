vmlinux
=====

在linux系统中，vmlinux(vmlinuz)是一个包含linux kernel的静态链接的可执行程序（elf,coff或a.out)。

编译linux内核源代码，最后得到的就是vmlinux，大小约为70M(2.6.38版本下)。

###vmlinuz

**vmlinux是未压缩的内核，vmlinuz是vmlinux的压缩文件。**

**vmlinuz是可引导的、压缩的内核。**   
**“vm”代表“Virtual Memory”。**    
Linux 支持虚拟内存，不像老的操作系统比如DOS有640KB内存的限制。Linux能够使用硬盘空间作为虚拟内存，因此得名“vm”。

在linux系统中，vmlinuz是可执行的Linux内核，它位于/boot/vmlinuz，它一般是一个软链接。

	例如, (Arch Linux, /boot/)：
	# file vmlinuz-linux
	vmlinuz-linux: Linux kernel x86 boot executable bzImage, version 4.2.5-1-ARCH (builduser@tobias) #1 SMP PREEMPT Tue Oct 27 08:13, RO-rootFS, swap_dev 0x4, Normal VGA
	
vmlinuz的生成有两种方式：

* 编译内核时通过`make zImage`创建      
	然后通过:    
	`cp /usr/src/linux-2.4/arch/i386/linux/boot/zImage /boot/vmlinuz`产生软链接。    
	zImage适用于小内核的情况，它的存在是为了向后的兼容性。
* 内核编译时通过命令`make bzImage`创建    
	然后通过:    
	`cp/usr/src/linux-2.4/arch/i386/linux/boot/bzImage /boot/vmlinuz”`\产生。    
	bzImage是压缩的内核映像，需要注意，bzImage不是用bzip2压缩的，bzImage中的bz容易引起误解，**bz表示“big zImage”**。
	
	
**内核文件中包含一个微型的gzip用于解压缩内核并引导它。**    
zImage(vmlinuz)和bzImage(vmlinuz)都是用gzip压缩的。它们不仅是一个压缩文件，而且在这两个文件的开头部分内嵌有gzip解压缩代码。所以不能用gunzip 或 gzip -dc解包vmlinuz。

**两者的不同之处在于，老的zImage解压缩内核到低端内存(第一个 640K)，bzImage解压缩内核到高端内存(1M以上)。**    
如果内核比较小，那么可以采用zImage或bzImage之一，两种方式引导的系统运行 时是相同的。大的内核采用bzImage，不能采用zImage。    

----

###initrd-x.x.x.img

**initrd是“initial ramdisk”的简写。initrd一般被用来临时的引导硬件到实际内核vmlinuz能够接管并继续引导的状态。**   

例如(Ubuntu 14.04):

	# root@Ubuntu:/boot# file initrd.img-3.13.0-32-generic
	initrd.img-3.13.0-32-generic: gzip compressed data, from Unix, last modified: Thu Oct 15 15:53:44 2015
	
initrd.img-3.13.0-32-generic主要是用于加载ext3等文件系统及scsi设备的驱动。

例如，计算机使用的是scsi硬盘，而内核vmlinuz中并没有这个scsi硬件的驱动，那么在装入scsi模块之前，内核不能加载根文件系统，但scsi模块存储在根文件系统的/lib/modules下。    
为了解决这个问题，可以**引导一个能够读实际内核的initrd内核并用initrd修正 scsi引导问题。initrd.img-3.13.0-32-generic是用gzip压缩的文件，initrd实现加载一些模块和安装文件系统等功能。**

initrd映象文件是使用mkinitrd创建的。    
这个命令是RedHat专有的。 其它Linux发行版或许有相应的命令。
这是个很方便的实用程序。具体情况请看帮助:man mkinitrd下面的命令创建initrd映象文件。

---

### uImage

zImage是一般情况下默认的压缩内核映像文件，压缩vmlinux，加上一段解压启动代码得到，只能从0X0地址运行。

uImage是u-boot使用bootm命令引导的Linux压缩内核映像文件格式，
使用工具mkimage对普通的压缩内核映像文件（zImage）加工而得。可以由bootm命令从任意地址解压启动内核。   
**uImage是u-boot专用的映像文件，它是在zImage之前加上一个长度为64字节的“头”，说明这个内核的版本、加载位置、生成时间、大小等信息；其0x40之后与zImage没区别。**

由于bootloader一般要占用0X0地址，所以，uImage相比zImage的好处就是可以和bootloader共存。


u-boot里面的mkimage工具来生成uImage（u-boot源码包/tools/mkimage.c )

	arm-linux-objcopy -O binary -R .note -R .comment -S vmlinux linux.bin
	gzip -9 linux.bin
	./mkimage -A arm -O linux -T kernel -C gzip -a 0x50008000 -e 0x50008000 -n "Linux Kernel Image" -d linux.bin.gz uImage

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

-a和-e后面跟的分别是image的载入地址(Load Address)和内核的入口地址(Enter Point)，两者可以一样，也可以不一样，依情况而定。   
当-a后面指定的地址和bootm xxxx后面的地址一样时，-e后面的地址必须要比-a后面的地址多0x40，也就是映像头的大小64个字节。因为当他们地址一样时，uboot是不会搬运映像的。    
当-a后面指定的地址和bootm xxxx后面的地址不一样时，uboot会将bootm xxxx地址处的映像搬运到-a指定的地址处，此时，-e和-a必须要一样，因为映像头并没有搬运过去，载入地址就是内核的入口地址。需要注意的是，因为uboot要重新搬运内核映像，所以要注意bootm xxxx的地址和-a之间的地址不要导致复制时的覆盖。    

------


此文件夹内提供已制作好的uImage:

	Image Name:   Linux Kernel Image
	Created:      Fri Nov  6 11:11:04 2015
	Image Type:   ARM Linux Kernel Image (gzip compressed)
	Data Size:    3742549 Bytes = 3654.83 kB = 3.57 MB
	Load Address: 50008000
	Entry Point:  50008000

内核源码: linux-2.6.38-20140106.tgz    

交叉编译工具: arm-linux-gcc-4.5.1-v6-vfp-20101103.tgz

U-Boot: u-boot-2011.06-for-MINI6410.tar.bz2
