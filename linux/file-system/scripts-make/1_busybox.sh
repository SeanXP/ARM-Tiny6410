#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 1_busybox.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/12/05 >
#	> Last Changed: 
#	> Description:		编译busybox并安装至rootfs
#################################################################

#!/bin/bash

# busybox源码压缩包
busybox_dir=`pwd`/busybox-1.24.1.tar.bz2
# 此配置文件，为make menuconfig后, 只配置了交叉编译工具前缀为'arm-linux-'后生成的, 再无其他配置
busybox_config=`pwd`/busybox-1.24.1.config
# 文件系统构造目录
rootfs=`pwd`/rootfs

# 解压busybox
echo "------decompress busybox(1.24.1)---------"
tar xjvf $busybox_dir
# 复制配置文件.config
echo "------copy configure file---------"
cp $busybox_config busybox-1.24.1/.config
cd busybox-1.24.1/
# 编译
make
# 安装至文件系统构造目录rootfs/
make CONFIG_PREFIX=$rootfs install
