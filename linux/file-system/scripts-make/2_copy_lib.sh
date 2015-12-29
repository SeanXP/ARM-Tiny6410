#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 2_copy_lib.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/12/05 >
#	> Last Changed: 
#	> Description:		拷贝交叉编译工具的库至文件系统目录
#################################################################

#!/bin/bash

# 交叉编译工具的库文件
arm_linux_gcc_lib=~/Code/cross/arm-2014.05/arm-none-linux-gnueabi/libc/lib
# 文件系统目录
rootfs=`pwd`/rootfs

# -d, 保留软链接
cp -d $arm_linux_gcc_lib/*.so* $rootfs/lib
