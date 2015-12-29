ARM-S3C6410 Bare-Metal Program
====

参考: 

* 韦东山. 嵌入式 Linux 应用完全开发手册[J]. 2008.
* [FriendlyARM - Tiny6410](http://www.arm9.net/tiny6410.asp)
* Linux平台下Tiny6410裸机程序开发指南.pdf

----

| No. | folder | description |
| :-: | :----: | :---------: |
| 1 | led | LED灯 |
| 2 | button | 按键 |
| 3 | clock | 时钟 |
| 4 | uart | UART |
| 5 | relocate | 重定位 |
| 6 | sdram | 内存 
| 7 | sd-no-os | SD卡启动 |
| 8 | mmu | MMU |
| 9 | nand | NAND Flash |
| 10 | buzzer | 蜂鸣器 |
| 11 | stdio | printf实现 |
| 12 | interrupt | 中断 |
| 13 | timer | 定时器 |
| 14 | watchdog | 看门狗 |

----

使用交叉编译工具: arm-linux-gcc-4.5.1-v6-vfp-20101103.tgz

其他版本的交叉编译工具亦可，但是可能存在一个问题：     
使用：arm-2014.05-29-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2    

交叉编译在`arm-linux-ld`时出现error:

	undefined reference to '__aeabi_unwind_cpp_pr0'

解决办法:
	
	arm-none-linux-gnueabi-gcc加上**-nostdlib**选项即可


-nostdlib, 不连接系统标准启动文件和标准库文件，只把指定的文件传递给连接器。

这个选项常用于编译内核、bootloader等程序，它们不需要启动文件、标准库文件。  

C语言程序执行的第一条指令, 并不是main函数。
生成一个C程序的可执行文件时, 编译器通常会在我们的代码上加几个被称为启动文件的代码(crt1.o、crti.o、crtend.o等)，
他们是标准库文件。这些代码设置C程序的堆栈等，然后调用main函数。
其依赖于操作系统，在裸板上无法执行，所以我们需要写一个(汇编代码，配置栈，并跳转至main函数)。
