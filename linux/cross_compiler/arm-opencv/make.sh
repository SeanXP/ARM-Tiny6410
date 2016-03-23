#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < make.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/03/23 >
#	> Last Changed:
#	> Description:      auto cross compile opencv for arm     
#
#   http://blog.sina.cn/dpool/blog/s/blog_60b330b801018zsh.html
#
#################################################################

#!/bin/bash
local_dir=`pwd`

# 1. 创建目录

source_dir=$local_dir/source
compressed_dir=$local_dir/compressed
install_dir=$local_dir/install
mkdir -p $source_dir $compressed_dir $install_dir

# 2. 下载源代码
wget http://zlib.net/zlib-1.2.8.tar.gz -P $compressed_dir
wget http://www.ijg.org/files/jpegsrc.v9b.tar.gz -P $compressed_dir
wget ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/libpng-1.6.21.tar.xz -P $compressed_dir
wget http://downloads.xvid.org/downloads/xvidcore-1.3.4.tar.gz -P $compressed_dir

cd $source_dir
# https://trac.ffmpeg.org/wiki/CompilationGuide/Quick/libx264
# How to quickly compile FFmpeg with libx264 (x264, H.264): https://trac.ffmpeg.org/wiki/How%20to%20quickly%20compile%20FFmpeg%20with%20libx264%20(x264,%20H.264)
git clone git://github.com/yasm/yasm.git
git clone git://git.videolan.org/x264.git
#git clone http://source.ffmpeg.org/git/ffmpeg.git
git clone https://github.com/FFmpeg/FFmpeg.git

# 3. 解压
cd $compressed_dir
tar xzvf zlib-*.tar.gz -C ../source/
tar xzvf jpegsrc*.tar.gz -C ../source/
tar xJvf libpng*.tar.xz -C ../source/
tar xzvf xvidcore*.tar.gz -C ../source/

# 4. 编译
cd $source_dir
zlib_dir=$source_dir/zlib*
jpeg_dir=$source_dir/jpeg*
libpng_dir=$source_dir/libpng*
libyasm_dir=$source_dir/yasm
libx264_dir=$source_dir/x264
libxvid_dir=$source_dir/libxvid
ffmpeg_dir=$source_dir/FFmpeg

zlib_install_dir=$install_dir/zlib
jpeg_install_dir=$install_dir/libjpeg
libpng_intall_dir=$install_dir/libpng
libyasm_install_dir=$install_dir/libyasm
libx264_install_dir=$install_dir/libx264
libxvid_install_dir=$install_dir/libxvid
ffmpeg_install_dir=$install_dir/ffmpeg
mkdir -p $zlib_install_dir $jpeg_install_dir $libpng_intall_dir $libyasm_install_dir $libx264_install_dir $ffmpeg_install_dir $libxvid_install_dir

# 4.1 编译zlib
cd $zlib_dir
./configure --prefix=$zlib_install_dir
sed -i 's/CC=gcc/CC=arm-linux-gcc/g' Makefile
sed -i 's/AR=ar/AR=arm-linux-ar/g' Makefile
sed -i 's/CPP=gcc/CPP=arm-linux-gcc/g' Makefile
sed -i 's/LDSHARED=gcc/LDSHARED=arm-linux-gcc/g' Makefile
make
make install

# 4.2 compile libjpeg
cd $jpeg_dir
./configure --host=arm-linux --prefix=$zlib_install_dir --enable-shared --enable-static
make
make install

# 4.3 compile libpng
cd $libpng_dir
./configure --host=arm-linux --prefix=$libpng_intall_dir --enable-shared --enable-static CFLAGS="-I$zlib_install_dir/include" LDFLAGS="-L$zlib_install_dir/lib"
#./configure --host=arm-linux --prefix=/home/hit413/Code/arm-opencv/install/libpng --enable-shared --enable-static CFLAGS="-I/home/hit413/Code/arm-opencv/install/zlib/include" LDFLAGS="-L/home/hit413/Code/arm-opencv/install/zlib/lib"
make
make install

# 4.4 compile libyasm
cd $libyasm_dir
./autogen.sh
./configure --host=arm-linux --prefix=libyasm_install_dir
#./configure --host=arm-linux --prefix=/home/hit413/Code/arm-opencv/install/libyasm
make
make install

# 4.5  compile libx264
cd $libx264_dir
CC=arm-linux-gcc ./configure --host=arm-linux --prefix=libx264_install_dir --enable-shared --enable-static --disable-asm
#CC=arm-linux-gcc ./configure --host=arm-linux --prefix=/home/hit413/Code/arm-opencv/install/libx264 --enable-shared --enable-static --disable-asm
make
make install

# 4.6 compile libxvid
cd $libxvid_dir/build/generic
./configure --host=arm-linux --prefix=$libxvid_install_dir --disable-assembly
#./configure --host=arm-linux --prefix=/home/hit413/Code/arm-opencv/install/libxvid --disable-assembly
make
make install

# 4.7 compile ffmpeg
cd $ffmpeg_dir
CC=arm-linux-gcc ./configure --host=arm-linux --arch=arm --prefix=/home/hit413/Code/arm-opencv/install/ffmpeg --enable-shared --disable-static --enable-gpl --enable-cross-compile --disable-stripping --enable-libx264 --enable-libxvid --enable-swscale --extra-cflags="-I/home/hit413/Code/arm-opencv/install/libx264/include" --extra-ldflags="-L/home/hit413/Code/arm-opencv/install/libx264/lib"
make
make install
