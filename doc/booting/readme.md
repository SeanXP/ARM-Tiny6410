S3C6410 Booting
=================

###CLOCK SOURCE SELECTION

The OM[4:0] pins determines the operating mode(启动模式, 如SD卡启动/NAND Flash启动等等) of S3C6410X when the external reset signal is asserted.   
As described in the table, the OM[0] selects the external clock source,   
i.e., if the OM[0] is 0, the XXTIpll (external crystal) is selected.  
Otherwise, XEXTCLK is selected.  

OM[4:0]决定了S3C6410的启动模式, 启动OM[0]决定了S3C6410的外部时钟源:  
	* OM[0] = 0, XEXTCLK, 外部时钟(external clock);
	* OM[1] = 1, XXTIpll, 外部晶振(external crystal;

The operating mode is mainly classified into six categories according to the boot device.    
The boot device can be among SROM, NOR, OneNAND, MODEM and Internal ROM.    

XSELNAND配置:   
* When NAND Flash device is used, XSELNAND pin must be 1, even if it is used as boot device or storage device.   
* When OneNAND Flash device is used, XSELNAND must be 0, even if it is used as boot device or storage device.   
* When NAND/OneNAND device is not used, XSELNAND can be 0 or 1.

6410支持SROM、ONENAND、IROM等几种Booting模式，其中最常用的是IROM下的NAND Flash和SD/MMC两种启动模式; 

6410 Booting模式配置，是通过XSELNAND、OM[4:0]、GPN[15:13]共同决定:   
* 选择NAND Flash Booting模式时，XSELNAND = 1; 
* 选择ONENAND时，XSELNAND = 0;
* 其它几种Booting模式，XSELNAND可以0或1。

参考: S3C6410X.PDF - Page 123 - 3.3.3 CLOCK SOURCE SELECTION

-------------
###SROM Booting

**寄存器配置： XSELNAND = X, OM[4:1]=0100 / 0101;**   
8bit或16bit SROM启动模式;  
这儿SROM一般情况下指的是Nor Flash，系统上电之后，
Boot镜像区为SROM控制器的第0个bank（128MB），即0x10000000~0x17FFFFFF地址的映射，
内核PC从0x0开始取指令实际是从SROM的bank0开始取指令运行。

-------------
###ONENAND Booting

**寄存器配置： XSELNAND = 0, OM[4:1]=0110;**    
ONENAND Flash启动模式，此时整个Boot镜像区0x0~0x07FFFFFF全部为静态存储器0x20000000~0x27FFFFFF地址镜像，
128MB一一对应，静态存储器0x20000000~0x27FFFFFF地址刚好是ONENAND Flash Bank0地址域，
内核PC从0x0开始取指令实际是从ONENAND Flash Bank0开始取指令运行。

-------------
###MODEM Booting  

**寄存器配置： XSELNAND = X, OM[4:1]=0111;**    

MODEM 启动模式，S3C6410的MODEM启动，指的外部HOST MODEM通过6410间接MODEM接口（indirect MODEM interface），
将启动代码传输到6410的内部 8KB stepping stone区域，然后通过设置协议寄存器（protocol register）系统控制寄存器（位于bank 0x0B）的bit0，激活s3c6410 MEODOM booting功能。

MODEM Booting基本流程：  
在6410上电之后，ARM内核PC从地址0x0开始取指令，此时Boot镜像区0x0开始的32KB为6410内部I_ROM地址 0x08000000~0x08007FFF的32KB数据的镜像，
所以当PC从0x0开始取指令执行，实际是从I_ROM 0x08000000处开始执行代码，
而I_ROM区32KB存放的是6410出厂固化的一段启动代码，
这段代码会自动根据是MODEM Boot启动配置，初始化MEDEM 主机接口，
从HOST处接收固件到8KB stepping stone区域，直至HOST激活MEODOM booting功能，
ARM内核PC跳转到stepping stone开始处执行代码。

有些资料在说明6410MODEM boot功能时候，有一种说法是6410通过直接MODEM接口（Direct MODEM interface）下载启动代码到“Direct MODEM interface”内部8KB双向RAM
然后启动s3c6410 MEODOM booting功能，这种说明不正确，
仔细阅读6410 使用说明，很容易知道6410 的MEDOM booting 功能是通过其indirect MODEM interface 直接下载启动代码到6410内部的8k stepping stone区域，
而后跳转到stepping stone继续运行（关于8K stepping stone在IROM Booting有详细说明）。

-------------
###IROM Booting

**寄存器配置： OM[4:1]=1111;**    

当OM[4:1]=1111，对应系统选择当IROM启动模式，
根据GPN[15:13]不同选择，IROM Boot又细分成SD/MMC、ONENAND、NAND Flash三种启动方式。
IROM Booting配置启动主要的特点是：首先上电之后Boot镜像区地址为0x00000000~0x00007FFF 的
32KB区域为6410内部的I_ROM区低32KB字节0x08000000~0x08007FFF区域数据的镜像，
ARM 内核从0x0取指令实际是从已经存储6410出厂固化代码的I_ROM区低32KB。
其次是I_ROM 32KB会根据GPN[15:13]的不同设定，执行不同的代码搬移工作，
比如当为NAND Flash启动设定，I_ROM 32KB会初始化NAND Flash控制器，搬移NAND Flash 的低8KB数据到6410内部的8k stepping stone区域，
然后跳转到8k stepping stone处运行代码。

ONENAND Flash(XSELNAND = 0，OM[4:1]=0110) Booting与IROM里面的ONENAND Flash Booting(XSELNAND =0，OM[4:1]=1111,GPN[15:13]=001)，
两种模式有本质上不同，前者是直接从ONENAND Flash直接运行启动代码，而后者是先是BL0搬移ONENAND Flash 前8K代码到Stepping stone，
而后跳转到Stepping stone运行。

#### IROM Booting 流程   

IROM Booting流程分2个阶段BL0和BL1:   

1. BL0为系统上电之后，最先执行I_ROM 32KB代码，搬移8KB（从NAND Flash、SD/MMC、ONENAND Flash等几者之一）代码到stepping stone，
然后跳转到stepping stone处执行，这段代码是6410出厂已经固化的代码，无需我们干预；
2. BL1为内核在stepping stone执行的流程，功能需要我们自己实现，
BL1主要是将除（NAND Flash、SD/MMC、ONENAND Flash等几者之一）8K之外的其他启动代码拷贝到SDRAM里面，
然后跳转到SDRAM执行，至此完成BL1。

BL1阶段，就是 /home/gxp/Code/ARM-Tiny6410/relocate/9.link_4096/ 所要实现的;

##### BL0 流程详细说明

参考: 《s3c6410 IROM Booting ApplicationNote V1.0》    
参考: http://blog.csdn.net/loongembedded/article/details/6637461    

系统上电或复位之后，BL0主要执行以下操作：   
* 禁止看门狗时钟；
* 初始化TCM(主要初始化TCM0 的 Secure Key和4KB堆，TCM1的8KB 堆栈)
* 初始化块设备拷贝函数
* 初始化堆栈
* 初始化PLL
* 初始化I-Cache
* 初始化堆
* Copy BL1到stepping stone
* 确认BL1的完整性
* 跳转到BL1
