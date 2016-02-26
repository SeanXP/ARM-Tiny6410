mjpg-streamer(ARM)
====

mjpg-streamer是开源的远程监控软件。

mjpg-streamer目录说明：
mjpg-streamer ：目录下提供了 的执行程序和各个输入输出设备组件
uvc-streamer  ： 目录下提供了  uvc-streamer的可执行目录
mjpeg-client：：  分别有 linux和windows 的客户端

## 脚本自动交叉编译

    ./make.sh

最后交叉编译的内容在`libjpeg_build/install`, `mjpg-streamer/mjpg-streamer`, 将其拷贝到对应ARM路径下:

    libjpeg_build/install/usr/local/ ----> (ARM) /usr/local/
    mjpg-streamer/mjpg-streamer      ----> (ARM) 任意路径

直接执行mjpg-streamer/mjpg-streamer/start.sh即可;


##ubuntu 编译：

	#!/bin/bash

	# 安装依赖库
	sudo apt-get update
	sudo apt-get install libjpeg8-dev imagemagick libv4l-dev

	# 下载代码
	git clone https://github.com/codewithpassion/mjpg-streamer.git
	cd mjpg-streamer/mjpg-streamer/
	make


##arm-linux版本交叉编译:

交叉编译jpeglib库
http://www.ridgesolutions.ie/index.php/2013/05/15/cross-compiling-libjpeg-for-linux-on-arm/


	$ mkdir libjpeg_build
	$ cd libjpeg_build
	$ wget http://www.ijg.org/files/jpegsrc.v9.tar.gz
	$ tar -xzvf jpegsrc.v9.tar.gz
	$ cd jpeg-9
	$ ./configure --host=arm CC=arm-linux-gcc
	$ make
	$ make install DESTDIR=/home/xxx/libjpeg_build/install

将install/拷贝到arm开发板;

##交叉编译mjpg-streamer

http://blog.sina.com.cn/s/blog_64d0b03c0101crzc.html

修改顶层makefile及plugins目录中的各级makefile将所有
CC=gcc  修改为  CC=arm-linux-gcc

并在plugins/input_uvc/Makefile中修改:

	LFLAGS += -ljpeg -L/home/hit413/jpeglib/install/usr/local/lib -I/home/hit413/jpeglib/install/usr/local/include

并在jpeg_utils.lo的规则下添加$(LFLAGS)

然后编译;

-----

参考:    
http://www.ridgesolutions.ie/index.php/2013/05/15/cross-compiling-libjpeg-for-linux-on-arm/      
http://blog.sina.com.cn/s/blog_64d0b03c0101crzc.html    
