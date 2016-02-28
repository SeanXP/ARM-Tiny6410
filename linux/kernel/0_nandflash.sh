#################################################################
#   Copyright (C) 2016 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 0_nandflash.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/02/24 >
#	> Last Changed: 
#	> Description:      移植NAND  Flash驱动, 拷贝FriendlyARM提供的NAND驱动相关代码;
#                     执行脚本前, 要将脚本与driver_nand/拷贝到linux内核代码的同级目录下;
#################################################################

#!/bin/bash
source_dir=driver_nand
target_dir=linux-2.6.38

# 覆盖代码
# SLC Flash分区信息在arch/arm/mach-s3c64xx/mach-mini6410.c;
# 修改SLC Flash分区信息
cp $source_dir/mach-mini6410.c  $target_dir/arch/arm/mach-s3c64xx/mach-mini6410.c
# MLC Flash分区信息static struct mtd_partition mini6410_nand_part_mlc[];
cp $source_dir/s3c_nand.c       $target_dir/drivers/mtd/nand/s3c_nand.c
cp $source_dir/nand_base.c      $target_dir/drivers/mtd/nand/nand_base.c
# FriendlyARM 闭源驱动, mtd分区可能写死在里面;
cp $source_dir/s3c_nand_mlc.fo  $target_dir/drivers/mtd/nand/s3c_nand_mlc.fo
# 添加config MTD_NAND_S3C, config MTD_NAND_S3C_DEBUG, config MTD_NAND_S3C_HWECC
cp $source_dir/Kconfig          $target_dir/drivers/mtd/nand/Kconfig
# 添加s3c_nand.o, s3c_nand_mlc.fo;
cp $source_dir/Makefile         $target_dir/drivers/mtd/nand/Makefile
# 增加s3c_nand.c的宏定义
cp $source_dir/regs-nand.h      $target_dir/arch/arm/plat-samsung/include/plat/regs-nand.h
# 替代配置文件.config, 等价make menuconfig后的结果;
cp $source_dir/tiny6410_config  $target_dir/.config
