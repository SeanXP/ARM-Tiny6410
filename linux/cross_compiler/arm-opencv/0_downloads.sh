#!/bin/bash
#################################################################
#   Copyright (C) 2016 Sean Guo. All rights reserved.
#														  
#	> File Name:        < 0_downloads.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/03/23 >
#	> Last Changed: 
#	> Description:      auto cross compile opencv for arm     
# > Dependent libraries: libz libjpeg libpng libyasm libx264 libxvid lffmpeg
#################################################################

#!/bin/bash
local_dir=`pwd`

# 1. 创建目录
source_dir=$local_dir/source
compressed_dir=$local_dir/compressed
mkdir -p $source_dir $compressed_dir

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
git clone https://github.com/FFmpeg/FFmpeg.git

# 3. 解压
cd $compressed_dir
tar xzvf zlib-*.tar.gz    -C ../source/
tar xzvf jpegsrc*.tar.gz  -C ../source/
tar xJvf libpng*.tar.xz   -C ../source/
tar xzvf xvidcore*.tar.gz -C ../source/
