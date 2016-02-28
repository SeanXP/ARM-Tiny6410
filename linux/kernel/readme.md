Linux 2.6.38 for Tiny6410
====

交叉编译工具: FriendlyARM [arm-linux-gcc-4.5.1-v6-vfp-20101103](http://www.arm9.net/download.asp)

Kernel: [Linux 2.6.38](ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.38.tar.bz2)

----

1. 下载linux-2.6.38的源码

		$ wget ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.38.tar.bz2
		$ tar xjvf linux-2.6.38.tar.bz2 -C .
2. 基本配置
	
		$ cd linux-2.6.38
		$ vi Makefile
	
	修改ARCH            ?= arm
		
		$ cp arch/arm/configs/s3c6400_defconfig .config
		$ make menuconfig

	General setup->Cross-compiler tool prefix: arm-linux-       
	System Type->[*] MINI6410   选上，其他的可以去掉

3. 编译

		$ make

	编译后得到vmlinux, 将其制作为uImage, 并通过uboot下载到ARM		
	
	[https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux#bootloader---u-boot]		

	配置bootargs:
	
		MINI6410 # setenv bootargs "console=ttySAC0,115200"
		MINI6410 # boot
	
	Linux Kernel可以启动，输出大量消息，不过因为其他内容尚未配置，目前不会完全启动。
	
4. 增加nand flash支持

    修改SLC Flash的mtd_partition: **arch/arm/mach-s3c64xx/mach-mini6410.c**

        static struct mtd_partition mini6410_nand_part[] = {
             [0] = {
                 .name   = "Bootloader",
                 .offset = 0,
                 .size   = (4 * 128 *SZ_1K),
                 .mask_flags    = MTD_CAP_NANDFLASH,
             },
             [1] = {
                 .name   = "Kernel",
                 .offset = (4 * 128 *SZ_1K),
                 .size   = (5 * SZ_1M),
                 .mask_flags    = MTD_CAP_NANDFLASH,
             },
             [2] = {
                 .name   = "File System",
                 .offset = MTDPART_OFS_APPEND,
                 .size   = MTDPART_SIZ_FULL,
             },
         };
	
    拷贝FriendlyARM提供的NAND相关代码, 参考: [http://www.arm9home.net/read.php?tid-14196.html]
    这里提供脚本: **0_nandflash.sh**

    编译内核并加载，可以看到内核输出:

        S3C NAND Driver, (c) 2008 Samsung Electronics
        S3C NAND Driver is using hardware ECC.
        NAND device: Manufacturer ID: 0xec, Chip ID: 0xd3 (Samsung NAND 1GiB 3,3V 8-bit)
        Creating 3 MTD partitions on "NAND 1GiB 3,3V 8-bit":
        0x000000000000-0x000000080000 : "Bootloader"
        0x000000080000-0x000000580000 : "Kernel"
        0x000000580000-0x000040000000 : "File System"
        ....
        ....
        ....
        VFS: Cannot open root device "(null)" or unknown-block(0,0)
        Please append a correct "root=" boot option; here are the available partitions:
        1f00             256 mtdblock0  (driver?)
        1f01            5120 mtdblock1  (driver?)
        1f02         1043200 mtdblock2  (driver?)
        Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)

    4 * 128 * 1024 = 8 * 16 ^ 4, 配置无误, Uboot预留空间512K, Kernel预留空间5M足够;

    这次Kernel panic的原因是没有根目录, 这也是接下来的移植目标;

----

参考:    
1. [linux-2.6.38到tiny6410的移植手册](http://www.arm9home.net/read.php?tid-14196.html)    
4. [基于Tiny6410上移植_kernel-2.6.38](http://wenku.baidu.com/view/5860607a7fd5360cba1adbc8.html)
