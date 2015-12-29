#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < create_rootfs_bash.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/12/05 >
#	> Last Changed: 
#	> Description:		自动创建Linux文件系统所需要的各类目录(rootfs)
#################################################################

#!/bin/bash
echo "------Create rootfs directions start...------"
mkdir rootfs
cd rootfs
echo "------Create root,dev,ect,home....----------"
mkdir root dev etc boot tmp var sys proc lib usr mnt home
mkdir etc/init.d etc/rc.d etc/sysconfig
mkdir usr/sbin usr/bin usr/lib usr/modules
mkdir mnt/etc mnt/jffs2 mnt/yaffs mnt/data mnt/temp
mkdir var/lib var/lock var/run var/tmp
echo "------Make node in dev/console, dev/null----"
sudo mknod -m 600 dev/console c 5 1
sudo mknod -m 600 dev/null c 1 3
echo "------Set sticky bit in tmp/----------------"
# 这里配置tmp的目录权限为1777, 开启sticky 位
# 对一个目录设置了sticky-bit之后，存放在该目录的文件仅准许其属主执行删除、 移动等操作。
chmod 1777 tmp
chmod 1777 var/tmp
echo "------Create rootfs directions end.---------"
