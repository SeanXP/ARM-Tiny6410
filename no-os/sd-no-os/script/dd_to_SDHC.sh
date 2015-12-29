#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < dd_to_SDHC.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/09/29 >
#	> Last Changed: 
#	> Description:		将编译的.bin文件通过dd命令烧写到SDHC的BL1区域;
#################################################################

#!/bin/bash

#					!!! 执行脚本前，必须明确指定要烧写的.bin文件(bin_file)与设备号(dev_SDHC) !!!
#					!!! 执行脚本前，必须明确指定要烧写的.bin文件(bin_file)与设备号(dev_SDHC) !!!
#					!!! 执行脚本前，必须明确指定要烧写的.bin文件(bin_file)与设备号(dev_SDHC) !!!
# 1. 指定SDHC在linux下的映射设备; 通过$ lsblk命令查看;
dev_SDHC="/dev/sdb" # 需要用户指定
bin_file="leds.bin"   # 指定要烧写的文件

echo 'SDHC ->' $dev_SDHC
echo "-------------- lsblk $dev_SDHC--------------"
lsblk | grep `basename $dev_SDHC`
echo '-------------------------------------------'

# 2. 通过fdisk查看SDHC的大小;
echo '[WARNING] fdisk, need sudoer authority'
echo "-------------- fdisk $dev_SDHC--------------"
#sudo fdisk -l | grep "Disk $dev_SDHC"
fdisk_string=`sudo fdisk -l | grep "Disk $dev_SDHC"` 
echo $fdisk_string
if [ -z "$fdisk_string" ]; then
	echo "Error! Can't find Device $dev_SDHC" 
	exit
fi

# eg. 
#Disk /dev/sdb: 7.3 GiB, 7822376960 bytes, 15278080 sectors


# fdisk 版本区别
# fdisk from util-linux 2.27
#-------------- fdisk /dev/sdb--------------
#Disk /dev/sdb: 14.4 GiB, 15462301696 bytes, 30199808 sectors
#-------------------------------------------

# fdisk (util-linux 2.20.1) 
# Disk /dev/sdb: 15.5 GB, 15462301696 bytes
echo '-------------------------------------------'

# 3. 计算SDHC的BL的起始位置 = 总字节数 - (1025 + 1 + 16)*512;
echo "SDHC BL1 = (All_Bytes - (1025 + 1 + 16)*512)"
ALL_Bytes=`echo $fdisk_string | cut -d ' ' -f 5`
echo "All_Bytes:$ALL_Bytes"
BL1_Start_Byte=`echo "($ALL_Bytes-(1025 + 1 + 16)*512)" | bc`
echo "BL1 Start Bytes:$BL1_Start_Byte"
echo '-------------------------------------------'

#4. 通过dd将$bin_file烧写到$dev_SDHC中
echo "-------------- dd $dev_SDHC--------------"
echo "dd $bin_file to SDHC($dev_SDHC)"
echo "sudo dd if=$bin_file of=$dev_SDHC seek=$BL1_Start_Byte bs=1"
sudo dd if=$bin_file of=$dev_SDHC seek=$BL1_Start_Byte bs=1
