mkconfig
================

U-Boot 编译步骤:

1. 进入 U-boot 源代码目录,执行:   
	`cd /opt/FriendlyARM/mini6410/linux/u-boot-mini6410`

2. 生成配置文件
	`make mini6410_nand_config-ram128`

3. 编译
	`make`

------

查看顶层Makefile文件:

	1944 mini6410_nand_config-ram128 :  unconfig
	1945     @$(MKCONFIG) mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram128

unconfig的定义:
	
	 342 unconfig:
	 343     echo 'ke'
	 344     @rm -f $(obj)include/config.h $(obj)include/config.mk \
	 345         $(obj)board/*/config.tmp $(obj)board/*/*/config.tmp

其中343行为我添加的一行代码；   
当执行`make mini6410_nand_config-ram128`命令时,
Makefile检索到1944行的mini6410_nand_config-ram128, 根据Makefile的规则, 要执行`make mini6410_nand_config-ram128`，
需要先执行`make unconfig`, 即删除旧的配置文件。    

之后执行执行`@$(MKCONFIG) mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram128`
即`./mkconfig mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram128`;

-----------

mkconfig脚本，根据输入参数，进行以下配置:

1. APPEND=no,即创建新配置文件
2. 根据参数$1设置开发板名称BOARD_NAME;
3. 输出消息"Configuring for xxxx board ..."
4. 进入./include目录, 准备进行配置(创建到平台/开发板相关的头文件链接)
5. 删除asm, 根据参数$2，决定使用arm-xxx链接到asm; 
6. 配置asm/arch目录, 根据参数$6选择链接特定架构目录;
7. 将特定cpu芯片的头文件(s3c6410.h)链接到通用头文件(regs.h);
8. 链接asm/proc目录;
9. 在include目录下, 生成config.mk文件，里面为配置信息;
10. 在include目录下, 生成config.h头文件，该头文件内包含特定芯片的头文件(configs/mini6410.h).此头文件为开发板的配置文件，内部包含很多需要配置的信息; 


------
	
	
	
	# $(MKCONFIG) mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram128
	# $0				$1	$2	$3		$4			$5		$6	  $7	$8
	APPEND=no		# Default: Create new config file
	BOARD_NAME=""	# Name to print in make output
	# $@ = mini6410 arm s3c64xx mini6410 samsung s3c6410 NAND ram128
	# $# = 8
	[ "${BOARD_NAME}" ] || BOARD_NAME="$1"
	# BOARD_NAME=mini6410
	echo "Configuring for ${BOARD_NAME} board which boot from $7 $8..."
	# Configuring for mini6410 board which boot from NAND ram128...
	cd ./include
	rm -f asm
	ln -s asm-$2 asm		# ln -s asm-arm
	rm -f asm-$2/arch		# rm -f asm-arm/arch
	# $6 = s3c6410
	ln -s ${LNPREFIX}arch-$6 asm-$2/arch	# ln -s arch-s3c6410 asm-arm/arch
	# $3 = s3c64xx
	# create link for s3c64xx SoC
	rm -f regs.h
	ln -s $6.h regs.h						# ln -s s3c6410.h regs.h
	rm -f asm-$2/arch						# rm -f asm-arm/arch
	ln -s arch-$3 asm-$2/arch				# ln -s arch-s3c64xx asm-arm/arch
	rm -f asm-$2/proc						# rm -f asm-arm/proc
	ln -s ${LNPREFIX}proc-armv asm-$2/proc  # ln -s proc-armv asm-arm/proc
	# Create include file for Make
	echo "ARCH   = $2" >  config.mk
	echo "CPU    = $3" >> config.mk
	echo "BOARD  = $4" >> config.mk
	# Create board specific header file
	> config.h		# Create new config file
	echo "/* Automatically generated - do not edit */" >>config.h
	# $7 = NAND
	case $7 in
	SD)
		echo "#define FRIENDLYARM_BOOT_MEDIA_SD"   >> config.h
		;;
	NAND)
		echo "#define FRIENDLYARM_BOOT_MEDIA_NAND" >> config.h
		;;
	*)
		;;
	esac
	# $8 = ram128
	case $8 in
	ram128)
		echo "#define FRIENDLYARM_BOOT_RAM128" >> config.h
		;;
	ram256)
		echo "#define FRIENDLYARM_BOOT_RAM256" >> config.h
		;;
	*)
		;;
	esac
	echo "#include <configs/$1.h>" >>config.h


