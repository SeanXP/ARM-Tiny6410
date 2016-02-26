#################################################################
#   Copyright (C) 2016 Sean Guo. All rights reserved.
#														  
#	> File Name:        < make.sh >
#	> Author:           < Sean Guo >		
#	> Mail:             < iseanxp+code@gmail.com >		
#	> Created Time:     < 2016/02/26 >
#	> Last Changed: 
#	> Description:
#################################################################

#!/bin/bash

# cross compile libjpeg
locale_dir=`pwd`
mkdir -p libjpeg_build
cd libjpeg_build
wget http://www.ijg.org/files/jpegsrc.v9.tar.gz
tar -xzvf jpegsrc.v9.tar.gz
cd jpeg-9
./configure --host=arm CC=arm-linux-gcc
make
libjpeg_install_dir=$locale_dir/libjpeg_build/install
mkdir -p $libjpeg_install_dir
make install DESTDIR=$libjpeg_install_dir

# cross compile mjpg-streamer
cd $locale_dir
git clone https://github.com/codewithpassion/mjpg-streamer.git
cd mjpg-streamer/mjpg-streamer
sed -i 's/CC = gcc/CC = arm-linux-gcc/g' Makefile
cd plugins
for dir in `find . -name 'Makefile'`;do
  sed -i 's/CC = gcc/CC = arm-linux-gcc/g' $dir
done
jpeg_lib_dir=$locale_dir/install/usr/local/lib
jpeg_include_dir=$locale_dir/install/usr/local/include
sed -i "/LFLAGS += -ljpeg/ a LFLAGS += -ljpeg -L$jpeg_lib_dir -I$jpeg_include_dir" input_uvc/Makefile
# 单引号不展开$, 双引号展开
sed -i 's/$(CC) -c $(CFLAGS) -o $@ jpeg_utils.c/$(CC) -c $(CFLAGS) $(LFLAGS) -o $@ jpeg_utils.c/g' input_uvc/Makefile
cd $locale_dir/mjpg-streamer/mjpg-streamer
make
