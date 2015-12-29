U-Boot.bin
=====

参考：[u-boot-2011.06 for mini6410](http://blog.chinaunix.net/uid-20543672-id-2748321.html)

源码：[https://github.com/tekkamanninja/u-boot-2011.06-for-MINI6410]
编译步骤:
	
	cd u-boot-2011.06-for-MINI6410
	make mini6410_config
	make
	
	
交叉编译环境(Friendly ARM 提供): arm-linux-gcc-4.5.1-v6-vfp-20101103.tgz



###u-boot-nand

u-boot-nand.bin

编译完以后，在u-boot-2011.06-for-MINI6410/目录下就生成u-boot-nand.bin文件，将其下载至nand flash的boot loader区域（例如0x0 ~ 0x80000的512K区域).

	
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
		


###u-boot-mmc

mmc_spl/u-boot-spl-16k.bin   
u-boot.bin  

U-boot因为在配置文件中已定义:

	#define MMC_UBOOT_POS_BACKWARD            (0x300000)
	#define MMC_ENV_POS_BACKWARD            (0x280000)
	#define MMC_BACKGROUND_POS_BACKWARD        (0x260000)

* mmc_spl/u-boot-spl-16k.bin烧写到BL1区（第一级引导，代码自拷贝部分）。
* u-boot.bin烧写到BL2（SD卡末尾向前3MB的位置）（0x300000）
* ENV的位置是在SD卡末尾向前2.5MB的位置（在BL2后0.5MB）（0x280000）
* 背景图片的位置在SD卡末尾向前0x260000的位置。
* 一般只需要烧写mmc_spl/u-boot-spl-16k.bin与u-boot.bin即可。

SD卡 - BL1在倒数第二个扇区, 假设总扇区数目为$(total_sectors):  

**mmc_spl写入扇区地址: write_addr = $(total_sectors) - 18;** 

	# dd if=mmc_spl/u-boot-spl-16k.bin of=/dev/sdb seek=write_addr bs=512;
实际运行时需要替换相应数字;

**u-boot.bin写入扇区地址: write_addr = $(total_sectors) - 6144 sectors (3MB);**

对于SDHC卡，最后的1024扇区是不识别的。
$(total_sector) = $(true_sector) - 1024;

eg. 

	Disk /dev/sdb: 7.4 GiB, 7948206080 bytes, 15523840 sectors

	u-boot-spl-16k.bin:
	15523840 - 1024 - 18 = 15522798 sectors
	u-boot.bin:
	15523840 - 1024 - 6144(3MB) = 15516672 sectors = 7944536064 bytes

`#sudo dd if=u-boot-spl-16k.bin of=/dev/sdb seek=15522798 bs=512`   
`#sudo dd if=u-boot.bin of=dev/sdb seek=7944536064 bs=1`   

dd具有扇区与字节的写入方式。

