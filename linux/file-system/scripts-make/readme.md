文件系统制作
=====

busybox: [busybox-1.24.1.tar.bz2](http://www.busybox.net/)   
arm-linux-gcc: arm-2014.05-29-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2

注意: 使用其他旧版arm-linxu-gcc会导致编译busybox-1.24.1失败;

注意: 执行`./2_copy_lib.sh`脚本前，需要根据实际的交叉编译器的路径修改脚本;

1. `./0_create_rootfs_bash.sh`,创建rootfs目录及子目录
2. `./1_busybox.sh`, 解压busybox并编译，并将其安装到rootfs;
3. `./2_copy_lib.sh`, 拷贝交叉编译工具的共享库文件，作为文件系统的命令运行的库;
4. `./3_config_files.sh`, 配置部分文件;

经过4个脚本，生成的rootfs可以直接使用(NFS挂载);   
参考: [NFS挂载文件系统](https://github.com/SeanXP/ARM-Tiny6410/tree/master/linux#file-system---文件系统)

提供生成的最小文件系统: rootfs.tar.bz2
