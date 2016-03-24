#################################################################
#   Copyright (C) 2015 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 1_make_libs.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/03/23 >
#	> Last Changed:
#	> Description:      auto cross compile opencv for arm     
# > Dependent libraries: libz libjpeg libpng libyasm libx264 libxvid lffmpeg
#
#   http://blog.sina.cn/dpool/blog/s/blog_60b330b801018zsh.html
#
#################################################################

#!/bin/bash
local_dir=`pwd`
source_dir=$local_dir/source
install_dir=$local_dir/install
mkdir -p $source_dir $install_dir

# 4. 编译
cd $source_dir
zlib_dir=$source_dir/zlib*
jpeg_dir=$source_dir/jpeg*
libpng_dir=$source_dir/libpng*
libyasm_dir=$source_dir/yasm
libx264_dir=$source_dir/x264
libxvid_dir=$source_dir/xvidcore
ffmpeg_dir=$source_dir/FFmpeg

zlib_install_dir=$install_dir/zlib
libjpeg_install_dir=$install_dir/libjpeg
libpng_intall_dir=$install_dir/libpng
libyasm_install_dir=$install_dir/libyasm
libx264_install_dir=$install_dir/libx264
libxvid_install_dir=$install_dir/libxvid
ffmpeg_install_dir=$install_dir/ffmpeg
mkdir -p $zlib_install_dir $jpeg_install_dir $libpng_intall_dir $libyasm_install_dir $libx264_install_dir $libxvid_install_dir $ffmpeg_install_dir 

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
./configure --host=arm-linux --prefix=$libjpeg_install_dir --enable-shared --enable-static
make
make install

# 4.3 compile libpng
cd $libpng_dir
export LC_ALL=en_US.UTF-8
CPPFLAGS="-I$zlib_install_dir/include" LDFLAGS="-L$zlib_install_dir/lib" ./configure --host=arm-linux --prefix=$libpng_intall_dir --enable-shared --enable-static
make
make install

# 4.4 compile libyasm
cd $libyasm_dir
./autogen.sh
./configure --host=arm-linux --prefix=$libyasm_install_dir
make
make install

# 4.5  compile libx264
cd $libx264_dir
CC=arm-linux-gcc ./configure --host=arm-linux --prefix=$libx264_install_dir --enable-shared --enable-static --disable-asm
make
make install

 4.6 compile libxvid
cd $libxvid_dir/build/generic
./configure --host=arm-linux --prefix=$libxvid_install_dir --disable-assembly
make
make install

# 4.7 compile ffmpeg
cd $ffmpeg_dir
./configure --target-os=linux --arch=arm --cc=arm-linux-gcc --prefix=$ffmpeg_install_dir --enable-shared --disable-static --enable-gpl --enable-cross-compile --disable-stripping --enable-libx264 --enable-libxvid --enable-swscale --extra-cflags="-I$libx264_install_dir/include -I$libxvid_install_dir/include" --extra-ldflags="-L$libx264_install_dir/lib -L$libxvid_install_dir/lib"
make
make install

# 5. copy files
cd $install_dir
mkdir -p lib/ include/
cp -r $ffmpeg_install_dir/lib/* $libjpeg_install_dir/lib/* $libpng_intall_dir/lib/* $libx264_install_dir/lib/* $libxvid_install_dir/lib/* $libyasm_install_dir/lib/* $zlib_install_dir/lib/* lib/
cp -r $ffmpeg_install_dir/include/* $libjpeg_install_dir/include/* $libpng_intall_dir/include/* $libx264_install_dir/include/* $libxvid_install_dir/include/* $libyasm_install_dir/include/* $zlib_install_dir/include/* include/
