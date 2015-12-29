#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 3_config_files.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/12/05 >
#	> Last Changed: 
#	> Description:		生成/etc目录内的重要文件
#################################################################

#!/bin/bash
# 当前目录, 内包含部分配置文件的拷贝
local=`pwd`
# 文件系统构造目录
rootfs=`pwd`/rootfs

cd $rootfs
cd etc/
# 创建mdev配置文件
touch mdev.conf
# 创建inittab文件
cp $local/inittab .

# 创建init.d/rcS启动脚本
cd init.d/
cp $local/rcS .
chmod a+x rcS

# 创建fstab挂载配置文件
cd ..
cp $local/fstab .

# 创建profile
cp $local/profile .
cp $local/hostname .
cp $local/group .
cp $local/passwd .
touch shadow
