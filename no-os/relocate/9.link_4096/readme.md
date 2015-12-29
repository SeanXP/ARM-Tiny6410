relocate 重定位
=================

IROM Booting启动下, BL0阶段CPU会自动拷贝Boot Device开头的部分数据
至CPU内部的SRAM(stepping stone), 且地址为0x0, CPU从stepping stone开始执行程序;    
Why? (因为CPU刚上电，并不知道是否连接有内存、外存等设备，因此必须在CPU内添加SRAM保证CPU的运行;)
但是由于要执行的程序可能超过stepping stone的大小，因此需要改变策略：    
拷贝至stepping stone的程序为一段小程序(非主程序)，负责配置好CPU并拷贝主程序到DRAM内存区域，最后跳转到DRAM执行主程序。

---------------------------

### IROM Booting 流程   

IROM Booting流程分2个阶段BL0和BL1:   

1. BL0为系统上电之后，最先执行I_ROM 32KB代码，搬移8KB（从NAND Flash、SD/MMC、ONENAND Flash等）代码到stepping stone，
然后跳转到stepping stone处执行，这段代码是6410出厂已经固化的代码，无需我们干预；
2. BL1为内核在stepping stone执行的流程，功能需要我们自己实现，
BL1主要是将除（NAND Flash、SD/MMC、ONENAND Flash等几者之一）8K之外的其他启动代码拷贝到SDRAM里面，
然后跳转到SDRAM执行，至此完成BL1。

BL1阶段，就是需要自己编程实现的。


---------------------------



