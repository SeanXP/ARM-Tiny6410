U-Boot
====

* mini2440_U-boot使用移植手册.pdf    
	U-Boot 使用及移植详细手册
* U-boot综述与源码分析.pdf
* bin, u-boot.bin文件
* Makefile(u-boot-1.1.6), Makefile文件分析

		mini6410_nand_config-ram256 :  unconfig
    		@$(MKCONFIG) mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram256
    		
* mkconfig, 脚本mkconfig分析

