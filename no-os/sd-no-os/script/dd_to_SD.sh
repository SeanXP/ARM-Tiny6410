#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < dd_to_SD.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/09/29 >
#	> Last Changed: 	< 2015/11/18 >
#	> Description:		将编译的.bin文件通过dd命令烧写到SD的BL1区域;
#################################################################

#!/bin/bash

# 1. 指定SD在linux下的映射设备; 通过$ lsblk命令查看;
#dev_SD=/dev/sda				# 需要用户指定
dev_SD=/dev/sdb				# 需要用户指定
# 指定要烧写的文件
bin_file="sd.bin"

echo 'SD ->' $dev_SD
echo "-------------- lsblk $dev_SD--------------"
lsblk | grep `basename $dev_SD`
echo '-------------------------------------------'

# 2. 通过fdisk查看SD的大小;
echo '[WARNING] fdisk, need sudoer authority'
echo "-------------- fdisk $dev_SD--------------"
#sudo fdisk -l | grep "Disk $dev_SD"
fdisk_string=`sudo fdisk -l | grep "Disk $dev_SD"` 
echo $fdisk_string

echo '-------------------------------------------'

# 3. 计算SD的BL的起始位置 = 总扇区数 - (1025 + 1 + 16);
echo "SD BL1 = (All_Sectors - (1 + 1 + 16))"
Sectors=`echo $fdisk_string | cut -d ' ' -f 7`
echo "Sectors:$Sectors"
((BL1_Start_Sector=$Sectors-(1 + 1 + 16)))
echo "BL1 Start Sector:$BL1_Start_Sector"
echo '-------------------------------------------'

#4. 通过dd将$bin_file烧写到$dev_SD中
echo "-------------- dd $dev_SD--------------"
echo "dd $bin_file to SD($dev_SD)"
echo "sudo dd if=$bin_file of=$dev_SD seek=$BL1_Start_Sector bs=512"
sudo dd if=$bin_file of=$dev_SD seek=$BL1_Start_Sector bs=512
