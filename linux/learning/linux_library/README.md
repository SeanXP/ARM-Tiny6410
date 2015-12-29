Linux 动态库/静态库
====
###库的基本概念
在windows平台和linux平台下都大量存在着库。    
本质上来说库是一种可执行代码的二进制形式，可以被操作系统载入内存执行。     
由于windows和linux的平台不同（主要是编译器、汇编器和连接器的不同），因此二者库的二进制是不兼容的。    
此处仅限于介绍linux下的库。
###库的种类
linux下的库有两种：静态库和共享库（动态库）。    
二者的不同点在于代码被载入的时刻不同。    
静态库的代码在编译过程中已经被载入可执行程序，因此体积较大。    
共享库的代码是在可执行程序运行时才载入内存的，在编译过程中仅简单的引用，因此代码体积较小。    

在linux下，库文件一般放在/usr/lib和/lib下，静态库的名字一般为libxxxx.a，其中xxxx是该lib的名称。     
动态库的名字一般为libxxxx.so.major.minor，xxxx是该lib的名称，major是主版本号， minor是副版本号。
 

###库存在的意义
库是别人写好的现有的，成熟的，可以复用的代码，你可以使用但要记得遵守许可协议。    
现实中每个程序都要依赖很多基础的底层库，不可能每个人的代码都从零开始，因此库的存在意义非同寻常。    
共享库的好处是，不同的应用程序如果调用相同的库，那么在内存里只需要有一份该共享库的实例。    

###查看依赖库
#####(linxu环境下)

ldd 简介：该命令用于打印出某个应用程序或者动态库所依赖的动态库     
涉及语法：ldd [OPTION]... FILE...    
其他详细说明请参阅 man 说明。    

ldd命令可以查看一个可执行程序依赖的共享库，
例如
	
	# ldd /lib/libx86.so.1
	
	linux-gate.so.1 =>  (0xb778a000)	libc.so.6 => /lib/i386-linux-gnu/libc.so.6 (0xb75c9000)	/lib/ld-linux.so.2 (0xb778b000)
可以看到x86库依赖于linux-gate库和libc库以及ld-linux库。
#####(Mac OS X)
	
	otool -L /usr/lib/libc++.1.dylib
	/usr/lib/libc++.1.dylib:
	/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 120.0.0)
	/usr/lib/libc++abi.dylib (compatibility version 1.0.0, current version 48.0.0)
	/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1197.1.1)
	
###查看可执行文件的大小
size 简介：该命令用于显示出可执行文件的大小     
涉及语法：size objfile...    
其他详细说明请参阅 man 说明。   

	$ size libc-2.19.so
	text    data     bss     dec     hex filename
	1721975   11468   11264 1744707  1a9f43 libc-2.19.so


###库的生成
#####动态库
动态库的后缀是.so，它由`gcc`加特定参数(-shared)编译产生。
####静态库
静态库的后缀是.a，它的产生分两步。    
Step 1.由源文件编译生成一堆.o，每个.o里都包含这个编译单元的符号表。    
Step 2.`ar`命令将很多.o转换成.a，成为静态库。

----


###一.动态库的基本概念

1. 动态链接库是程序运行时加载的库，当动态链接库正确安装后，所有的程序都可以使用动态库来运行程序。**动态链接库是目标文件的集合**，目标文件在动态链接库中的组织方式是按照特殊方式形成的。库中函数和变量的地址是相对地址，不是绝对地址，其真实地址在调用动态库的程序加载时形成。

2. 动态链接库的名称有**别名（soname）, 真名(realname)和链接名（linker name）**。   
	别名由一个前缀lib,然后是库的名字，再加上一个后缀“.so”构成。真名是动态链接库真实名称，一般总是在别名的基础加上一个小版本号，发布版本等构成。除此之外，还有一个链接名，即程序链接时使用的库的名字。    
	如下面的例子:
	* 别名(libtiger.so)
	* 真名(libtiger.so.1, libtiger.so.2)
	* 链接名(ltiger)

3. 在动态链接库安装的时候，总是复制文件到某个目录下，然后用一个软连接生成别名，在库文件进行更新的时候，仅仅更新软链接即可。

4. 共享库是目标文件的集合，但是这些目标文件是由编译程序按照特殊方式生成的。对象模块的每个地址（变量引用和函数调用）都是相对地址，不是绝对地址。因此允许在运行程序的时候，可以动态加载和执行共享模块。

5. **当一个程序使用共享函数库时，在连接阶段并不把函数代码连接进来，而只是链接函数的一个引用。**当最终的函数导入内存开始真正执行时，函数引用被解析，共享函数库的代码才真正导入到内存中。这样，共享链接库的函数就可以被许多程序同时共享，并且只需存储一次就可以了。共享函数库的另一个优点是，它可以独立更新，与调用它的函数毫不影响。

###二.生成和使用动态库

创建一个测试文件夹, 创建几个C语言文件。
	
	$ mkdir src
main.c文件中的内容:

	#include <stdio.h>
	#include ”tiger.h”
 
	int  main(void)
	{
		printf(“sum =%d\n”,add(5,3));
		printf(“sub= %d\n”,sub(5,3));
 
		return 0;     
	}
这里main.c程序, 使用了tiger.h头文件给定的接口add(), sub().

头文件提供接口, 动态库提供函数的实现, 是固定的搭配。

tiger.h头文件:

	#ifndef __TIGER__
	#define __TIGER__
 
	int add(int  a,int  b);
	int sub(int  a,int  b);
 		
	#endif
	
具体函数实现代码: (add.c)

	int  add(int a, int b)
	{
		return a +b;
	}	
sub.c:

	int  sub(int a, int b)
	{
		return  a - b;
	}

1. 生成目标文件add.o , sub.o ;    
	`gcc -fpic  -c  add.c`   
	`gcc -fpic -c sub.c`
	
	选项-c明确指出编译程序要生成.o目标文件。    
	而选项-fpic使得输出的对象模块式按照可重定位地址（relocatable addressing）方式生成的。表示编译为位置独立的代码，不用此选项的话编译后的代码是位置相关的。 
	缩写pic代表位置独立代码（position independent code）。
2. 生成动态库libtiger.so    
	`gcc  add.o sub.o -shared –o  libtiger.so`
	
	生成动态库libtiger.so,libtiger.so就是我们生成的目标动态库。     
	通常，连接程序要为主函数main()定位，并用它作为程序的入口，但这里编译的动态库模块没有这样的入口函数，因此需要指定选项-shared，防止出错信息。
	我们以后使用动态库和main.c程序生成可执行程序。   
	前两步也可以一步完成   `gcc add.c sub.c -fpic -shared -o libtiger.so`
	
3. 使用动态链接库。

	在编译程序时，使用动态链接库和静态库是一致的，使用”-l库名”的方式，在生成可执行文件的时候会链接库文件。	
	
	`gcc main.c -ltiger -L./ -o main`
	
	其中, -l后接需要连接的库名, -L后接库对应的搜索路径。   
	Linux系统下的动态库命名方式是`lib*.so`,而在链接时表示位`-l*`,`*`是自己命名的库名。(别名(libtiger.so),链接名(ltiger))
	
4. 运行含有动态连接库的程序。    
	直接运行程序
	`./main`
	将导致错误: "error while loading shared libraries: libtiger.so: cannot open shared object file: No such file or directory"    
	这是因为程序运行时没有找到动态链接库造成的。   
	程序编译时链接动态库和运行时使用动态链接库的概念是不同的，在运行时，程序链接的动态链接库需要在系统目录下才行。
	这里运行main程序时, 因为在系统默认的库搜索路径中, 没有搜索到对应的库libtiger.so, 导致运行错误。
	
	解决方案：
	1. 拷贝程序所需的库文件至linux的库搜索路径/lib目录中。(需要超级用户权限)
	2. 添加库搜索路径。可以将库的路径加到环境变量LD_LIBRARY_PATH中实现：
		
			export LD_LIBRARY_PATH= `pwd`:$LD_LIBRARY_PATH
			
####定位共享库文件
当系统加载可执行代码时候，能够知道其所依赖的库的名字，但是还需要知道绝对路径。    
此时就需要系统动态载入器(dynamic linker/loader)。    
对于elf格式的可执行程序，是由`ld-linux.so*`来完成的，它先后搜索elf文件的 `DT_RPATH`段，环境变量`LD_LIBRARY_PATH`，`/etc/ld.so.cache`文件列表，`/lib/`，`/usr/lib`目录找到库文件后将其载入内存。     
 
如：`export LD_LIBRARY_PATH=’pwd’`
将当前文件目录添加为共享目录.

####添加共享库文件
如果安装在/lib或者/usr/lib下，那么ld默认能够找到，无需其他操作。    
如果安装在其他目录，需要将其添加到/etc/ld.so.cache文件中，步骤如下：   
 
1. 编辑/etc/ld.so.conf文件，加入库文件所在目录的路径。    
2. 运行ldconfig，该命令会重建/etc/ld.so.cache文件

----

###静态库生成

`cd src/`    
`ar rcs libtiger.a add.o sub.o`   
`gcc main.c -ltiger -L. -o main`   
`./main`

####ar
ar 简介：处理创建、修改、提取静态库的操作     
涉及选项：    

* t - 显示静态库的内容 
* r[ab][f][u] - 更新或增加新文件到静态库中 
* [s] - 创建文档索引 
* ar -M [<mri-script] - 使用 ar 脚本处理 

其他详细说明请参阅 man 说明。


静态库制作完了，如何使用它内部的函数呢？    
只需要在使用到这些公用函数的源程序中 **包含这些公用函数的原型声明**，然后在用gcc命令生成目标文件时指明静态库名，gcc将会从静态库中将公用函数连接到目标文件中。    

静态链接库的一个缺点是，如果我们同时运行了许多程序，并且它们使用了同一个库函数，这样，在内存中会大量拷贝同一库函数。这样，就会浪费很多珍贵的内存和存储空间。使用了共享链接库的Linux就可以避免这个问题。

----
