#################################################################
#   Copyright (C) 2016 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 1_dm9000_nfs.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/02/28 >
#	> Last Changed: 
#	> Description:      移植dm9000网卡驱动与NFS驱动至Linux Kernel - 2.6.38
#################################################################

#!/bin/bash
dm9000_dir=dm9000
nfs_dir=nfs
kernel_dir=linux-2.6.38

# struct dm9000_plat_data新增成员变量: unsigned char   param_addr[6];
cp $dm9000_dir/dm9000.h $kernel_dir/include/linux/dm9000.h
cp $dm9000_dir/dm9000.c $kernel_dir/drivers/net/dm9000.c
cp $dm9000_dir/mach-mini6410.c $kernel_dir/arch/arm/mach-s3c64xx/mach-mini6410.c
# 等价make menuconfig - 配置网络相关, 并添加NFS挂载支持;
cp $dm9000_dir/tiny6410_config $kernel_dir/.config
