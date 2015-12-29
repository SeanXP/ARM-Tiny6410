SD卡启动U-boot
=====

6410的手册上说，可以从nandflash、onenand、SD卡启动，没有专用的烧录工具的情况下，只有SD卡启动是可以考虑的。

手册上看到，SD卡启动，实际上是先执行片内IROM中的一段程序，该程序从SD卡中读取代码，写到stepping stone 中，stepping stone是位于0x0c000000、size为8K的片内内存，代码写入stepping stone后，跳到0x0c000000处继续执行程序。

这里使用的uboot并非uboot官方发布的uboot代码，而是友善之臂官方提供的u-boot-mini6410(u-boot-1.1.6)；

1. 配置启动方式。
虽然支持uboot启动，但是uboot代码里不叫SD启动方式，而是叫**movinand启动方式**，在`incluede/configs/smdk6410.h`中就有这个选项，在代码里关闭nand启动，打开movinand启动。

		//#define CONFIG_BOOT_NOR
		//#define CONFIG_BOOT_NAND       注释nand启动
		#define CONFIG_BOOT_MOVINAND   打开movinand启动
		//#define CONFIG_BOOT_ONENAND
		//#define CONFIG_BOOT_ONENAND_IROM
		
		//打开movinand选项，使uboot支持movinand的操作
		#define  CONFIG_NAND
		//#define CONFIG_ONENAND
		#define CONFIG_MOVINAND
		
2. 根据具体板子选择是否配置HSMMC_CHANNEL。uboot中假设SMDK6410在使用SD方式的时候是从CH0启动的，如果实际的开发板为CH1，则需要在`incluede/movi.h`中进行修改。否则，BL0引导BL1代码并运行，在准备复制BL2至SDRAM时无法从CH0检测到SD卡，从而导致boot失败。
3. 制作u-boot.bin。由于u-boot.bin需要在SD卡的BL1 & BL2位置分别存储，这里需要进行部分配置：**u-boot.bin = u_boot_256k + u_boot_8k;**   
	
		cat u-boot.bin >> temp
		cat u-boot.bin >> temp
		split -b 256k temp
		mv xaa u-boot_256k.bin
		split -b 8k u-boot.bin
		mv xaa u-boot_8k.bin
		cat u-boot_256k.bin >> u-boot_mmc.bin
		cat u-boot_8k.bin >> u-boot_mmc.bin
		
	原理：	
	可以通过烧写工具IROM_Fusing_tools直接烧写，也可以通过dd命令行进行烧写。dd烧写时要计算好存放地址，保证u_boot_8k正好存放在BL1开头，而u_boot_256k存放在BL1前面，即BL2区域。      
	从网上可以下载到IROM_Fusing_tools的源码，在按下这个软件的start控件后，先是读取这个SD卡的第一个扇区，也就是这个磁盘的MBR 扇区，判断是不是FAT32格式的磁盘（这也是为什么用来做启动的SD必须格式化为FAT32格式），接着获取总的扇区数目TOTAl_SECOTR，并将所要烧写的bin文件烧写到磁盘的这个扇区：TOTAL_SECTOR - 2 - SIZE_OF_IMAGE/512。 
	其中TOTAl_SECTOR是这个磁盘总的扇区数目；SIZE_OF_IMAGE/512是这个bin文件将要占据的扇区数（这里是以512为扇区大小的，因此对于扇区更大的SD卡也就没办法使用了，而现在的大容量SD都可能使用了2K甚至4K的扇区，除非修改这个程序，并同步地在uboot中修改程序）；     
	至于2则是保留的2个扇区，至于为什么要保留这2个扇区，需要分析uboot的源码情况，下面将做进一步的阐述。    
	在SD启动方式下，S3C6410内部的IROM程序BL0首先运行，并将SD中的最后18个扇区开始的16个扇区内容复制到片内的8K SRAM，也就是SteppingStone，接着跳转到这块SRAM的开始地址开始运行，这8K的代码实际上就是上面u-boot_mmc.bin这个文件的最后8K，也是u-boot.bin的最开始8K代码，这段代码也叫BL1。从BL0跳转到BL1的时候uboot也就接管了CPU。    
	在 u-boot 中 SD 卡又叫做 MoviNand，如果要从 IROM + SD 启动，需要在 include/configs/mini6410.h 中定义 CONFIG_BOOT_MOVINAND 宏，相应地在 cpu/s3c64xx/start.S 中使用如下代码完成代码的拷贝。
	当复制完BL2后便会跳转到BL2的start_armboot这个C语言函数中运行了。
	
----

###探索 IROM + SD 启动

在 u-boot 中 SD 卡又叫做 MoviNand，如果要从 IROM + SD 启动，需要在`include/configs/mini6410.h` 中定义 CONFIG_BOOT_MOVINAND 宏，相应地在 `cpu/s3c64xx/start.S `中使用如下代码完成代码的拷贝。

	#ifdef CONFIG_BOOT_MOVINAND
	ldr   sp, _TEXT_PHY_BASE
	bl    movi_bl2_copy
	b     after_copy
	#endif
	
movi_bl2_copy 负责将 SD 卡内的 BL2 代码复制到 DDR，BL2 包含了整个 u-boot 代码，movi_bl2_copy 实际上是调用了CopyMovitoMem。

	CopyMovitoMem(HSMMC_CHANNEL, MOVI_BL2_POS, MOVI_BL2_BLKCNT, (uint *)BL2_BASE, MOVI_INIT_REQUIRED);

* HSMMC_CHANNEL：SD/MMC 通道号。
* MOVI_BL2_POS：数据的源地址，即 BL2 代码在 SD 卡中的起始地址，必须以 block(扇区) 为单位。
* MOVI_BL2_BLKCNT：需要复制的 blocks(扇区数目)。
* BL2_BASE：数据的目的地址，也就是 DDR 中的地址。
* MOVI_INIT_REQUIRED：是否需要重新初始化，一般设为 0 即可。

CopyMovitoMem 的定义在 `/include/movi.h` 中：

	#define CopyMovitoMem(a,b,c,d,e)     (((int(*)(int, uint, ushort, uint *, int))(*((uint *)(TCM_BASE + 0x8))))(a,b,c,d,e))

这个定义实际上是调用了位于 TCM_BASE + 0x8 这个地址的函数指针(TCM_BASE 的值为 0x0C004000)，这是 samsung 在 S3C6410 片内实现的代码。

----

## 测试结果

####SD卡测试

使用相同的SDHC卡(8G)，使用友善之臂提供的烧写工具SD-Flasher.exe烧写u-boot-mini6410(u-boot-1.1.6)到SDHC卡，串口输出：
	
	U-Boot 1.1.6 (Oct 18 2011 - 15:36:18) for FriendlyARM MINI6410


	CPU:     S3C6410@532MHz
         Fclk = 532MHz, Hclk = 133MHz, Pclk = 66MHz, Serial = CLKUART (SYNC Mode)
	Board:   MINI6410
	DRAM:    256 MB
	Flash:   0 kB
	NAND:    1024 MB
	MMC:     0 MB

系统卡死在最后一行*MMC:     0 MB*。

使用友善之臂提供的**Superboot-6410.bin**，同样的烧写过程，就可以启动系统。
经过测试，至少SDHC卡没有问题。

使用8G的SDHC卡，通过上述的dd烧写方式写入u-boot-sd.bin，已久无法启动。

**考虑u-boot版本过久，对sdhc不够支持，准备尝试移植新版本的u-boot.**

----

### 选择其他U-Boot

这里选择**Tekkaman Ninja**的 [**u-boot-2011.06 for mini6410**](http://blog.chinaunix.net/uid-20543672-id-2748321.html)

[Tekkaman Ninja - github](https://github.com/tekkamanninja/u-boot-2011.06-for-MINI6410)

1.下载源代码  
2. 交叉编译  
3. 得到u-boot.bin, mmc_spl/u-boot-spl-16k.bin, nand_spl/u-boot-spl-16k.bin;其中mmc_spl用于下载到SD卡，nand用于下载到nand.  
4. 使用linux命令dd写入数据: 

	因为在配置文件中已定义:
	#define MMC_UBOOT_POS_BACKWARD            (0x300000)
	#define MMC_ENV_POS_BACKWARD            (0x280000)
	#define MMC_BACKGROUND_POS_BACKWARD        (0x260000)

* mmc_spl/u-boot-spl-16k.bin烧写到BL1区（第一级引导，代码自拷贝部分）。
* u-boot.bin烧写到BL2（SD卡末尾向前3MB的位置）（0x300000）
* ENV的位置是在SD卡末尾向前2.5MB的位置（在BL2后0.5MB）（0x280000）
* 背景图片的位置在SD卡末尾向前0x260000的位置。
* 一般只需要烧写mmc_spl与u-boot即可。

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

----


		