#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < make.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2015/12/15 >
#	> Last Changed:     < 2016/02/19 >
#	> Description:
#################################################################

#!/bin/bash
local_dir=`pwd`

# 1. 创建目录

source_dir=$local_dir/source
compressed_dir=$local_dir/compressed
install_dir=$local_dir/install
mkdir -p $source_dir $compressed_dir $install_dir

# 2. 下载源代码
wget http://mirrors.nycbug.org/pub/OpenBSD/OpenSSH/portable/openssh-7.1p1.tar.gz -P $compressed_dir
wget https://www.openssl.org/source/openssl-1.0.2f.tar.gz -P $compressed_dir
wget http://zlib.net/zlib-1.2.8.tar.gz -P $compressed_dir

# 3. 解压
cd $compressed_dir
tar xzvf zlib-*.tar.gz -C ../source/.
tar xzvf openssh-*.tar.gz -C ../source/.
tar xzvf openssl-*.tar.gz -C ../source/.

# 4. 编译
cd $source_dir
zlib_dir=$source_dir/zlib*
openssh_dir=$source_dir/openssh*
openssl_dir=$source_dir/openssl*

zlib_install_dir=$install_dir/zlib
openssl_install_dir=$install_dir/openssl
mkdir -p $zlib_install_dir $openssl_install_dir

# 4.1 编译zlib
cd $zlib_dir
./configure --prefix=$zlib_install_dir
sed -i 's/CC=gcc/CC=arm-linux-gcc/g' Makefile
sed -i 's/AR=ar/AR=arm-linux-ar/g' Makefile
sed -i 's/CPP=gcc/CPP=arm-linux-gcc/g' Makefile
sed -i 's/LDSHARED=gcc/LDSHARED=arm-linux-gcc/g' Makefile
make
make install

# 4.2 编译openssl
cd $openssl_dir
./*onfigure --prefix=$openssl_install_dir  os/compiler:arm-none-linux-gnueabi-gcc
make 
make install

# 4.3 编译openssh
cd $openssh_dir
./configure --host=arm-none-linux-gnueabi --with-libs --with-zlib=$zlib_install_dir --with-ssl-dir=$openssl_install_dir --disable-etc-default-login CC=arm-none-linux-gnueabi-gcc AR=arm-none-linux-gnueabi-ar
make
#注: openssh 不需要make install

# 5. 整理文件
# 5.1 拷贝openssh
arm_dir=$local_dir/arm-rootfs 
mkdir -p $arm_dir
cd $openssh_dir
mkdir -p $arm_dir/usr/local/bin
cp scp  sftp  ssh  ssh-add  ssh-agent  ssh-keygen  ssh-keyscan $arm_dir/usr/local/bin
mkdir -p $arm_dir/usr/local/etc
cp moduli ssh_config sshd_config $arm_dir/usr/local/etc
mkdir -p $arm_dir/usr/local/libexec
cp sftp-server ssh-keysign $arm_dir/usr/local/libexec
mkdir -p $arm_dir/usr/local/sbin
cp sshd $arm_dir/usr/local/sbin
mkdir -p $arm_dir/var/empty
sudo chown root:root $arm_dir/var/empty
sudo chmod 755 $arm_dir/var/empty

# 5.2 拷贝zlib
cd $zlib_install_dir
mkdir -p $arm_dir/lib
cp -r lib/libz.so.1.2.* $arm_dir/lib
cd $arm_dir/lib
ln -s libz.so.1.2.* libz.so.1
ln -s libz.so.1.2.* libz.so
# 5.3 拷贝openssl
cd $openssl_install_dir
mkdir -p $arm_dir/usr/bin
cp -r bin/* $arm_dir/usr/bin
mkdir -p $arm_dir/usr/lib
cp -r lib/*.a $arm_dir/usr/lib

# 6. 生成ssh key
cd $local_dir
mkdir -p ssh_key
cd ssh_key
ssh-keygen -t rsa -f ssh_host_rsa_key -N ""
ssh-keygen -t dsa -f ssh_host_dsa_key -N ""
ssh-keygen -t ecdsa -f ssh_host_ecdsa_key -N ""
ssh-keygen -t ed25519 -f ssh_host_ed25519_key -N ""
chmod 600 ssh_host*key
cp ssh_host*key $arm_dir/usr/local/etc/
