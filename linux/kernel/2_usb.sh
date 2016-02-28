#################################################################
#   Copyright (C) 2016 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 2_usb.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/02/28 >
#	> Last Changed: 
#	> Description:      配置内核增加其他USB设备驱动
#################################################################

#!/bin/bash

usb_dir=usb
kernel_dir=linux-2.6.38

# 添加void s3c_otg_phy_config(int enable)函数定义;
cp $usb_dir/mach-mini6410.c $kernel_dir/arch/arm/mach-s3c64xx/mach-mini6410.c
cp $usb_dir/ohci-s3c2410.c $kernel_dir/drivers/usb/host/ohci-s3c2410.c
cp $usb_dir/tiny6410_config $kernel_dir/.config
