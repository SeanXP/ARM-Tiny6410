Cross Compiler - 交叉编译器
====

交叉编译器（英语：Cross compiler）是指一个在某个系统平台下可以产生另一个系统平台的可执行文件的编译器。交叉编译器在目标系统平台（开发出来的应用程序序所运行的平台）难以或不容易编译时非常有用。



###为什么要用到交叉编译器？

**嵌入式系统往往资源都是很有限的。**   
（比如，一些嵌入式设备的内存往往是几兆字节，且只有FLASH而没有硬盘这种大容量存储设备。）

在这种极其有限的资源环境中，我们不可能将我们的开发工具安装在嵌入式设备中，在上面进行编码、调试最后发布软件。

现实情况是，嵌入式系统的开发是在**主机(Host Machine，主机。一般为个人计算机，具有丰富的计算资源、存储资源等)**上进行的，在主机上进行交叉编译，得到可以在**目标机（Target Machine, 目标机。一般为嵌入式设备，例如ARM板）**上运行的可执行文件。

让一个编译器运行在x86主机上，却编译出可以在ARM上运行的可执行程序，这种编译器就是**交叉编译器（cross compiler）**，而采用交叉编译器进行编译就是指**交叉编译（cross compiling）**，而交叉编译器相关的环境就是指**交叉编译环境（cross compiling environment）**。

----

###arm交叉编译器gnueabi、none-eabi、arm-eabi、gnueabihf、gnueabi区别

参考：[http://www.veryarm.com/296.html]

####命名规则

交叉编译工具链的命名规则为：arch [-vendor] [-os] [-(gnu)eabi]

* arch - 体系架构，如ARM，MIPS
* vendor - 工具链提供商
* os - 目标操作系统
* eabi - 嵌入式应用二进制接口（Embedded Application Binary Interface）

根据对操作系统的支持与否，ARM GCC可分为支持和不支持操作系统，如

* arm-none-eabi：这个是没有操作系统的，自然不可能支持那些跟操作系统关系密切的函数，比如fork(2)。他使用的是newlib这个专用于嵌入式系统的C库。
* arm-none-linux-eabi：用于Linux的，使用Glibc
 
####实例

1. **arm-none-eabi-gcc**    
（ARM architecture，no vendor，not target an operating system，complies with the ARM EABI）   
	用于编译 ARM 架构的裸机系统（包括 ARM Linux 的 boot、kernel，**不适用编译 Linux 应用 Application**），一般适合 ARM7、Cortex-M 和 Cortex-R 内核的芯片使用，所以不支持那些跟操作系统关系密切的函数，比如fork(2)，他使用的是 newlib 这个专用于嵌入式系统的C库。

2. **arm-none-linux-gnueabi-gcc**    
(ARM architecture, no vendor, creates binaries that run on the Linux operating system, and uses the GNU EABI)    
主要用于基于ARM架构的Linux系统，可用于编译 ARM 架构的 u-boot、Linux内核、linux应用等。arm-none-linux-gnueabi基于GCC，使用Glibc库，经过 Codesourcery 公司优化过推出的编译器。arm-none-linux-gnueabi-xxx 交叉编译工具的浮点运算非常优秀。一般ARM9、ARM11、Cortex-A 内核，带有 Linux 操作系统的会用到。

3. arm-eabi-gcc    
	Android ARM 编译器。

4. armcc    
	ARM 公司推出的编译工具，功能和 arm-none-eabi 类似，可以编译裸机程序（u-boot、kernel），但是不能编译 Linux 应用程序。armcc一般和ARM开发工具一起，Keil MDK、ADS、RVDS和DS-5中的编译器都是armcc，所以 armcc 编译器都是收费的（爱国版除外，呵呵~~）。

5. arm-none-uclinuxeabi-gcc 和 arm-none-symbianelf-gcc       
	* arm-none-uclinuxeabi 用于uCLinux，使用Glibc。
	* arm-none-symbianelf 用于symbian，没用过，不知道C库是什么 。


####Codesourcery

Codesourcery推出的产品叫Sourcery G++ Lite Edition，其中基于command-line的编译器是免费的，在官网上可以下载，而其中包含的IDE和debug 工具是收费的，当然也有30天试用版本的。

目前CodeSourcery已经由明导国际(Mentor Graphics)收购，所以原本的网站风格已经全部变为 Mentor 样式，但是 Sourcery G++ Lite Edition 同样可以注册后免费下载。

Codesourcery一直是在做ARM目标 GCC 的开发和优化，它的ARM GCC在目前在市场上非常优秀。

####ABI 和 EABI

ABI：二进制应用程序接口(Application Binary Interface (ABI) for the ARM Architecture)。在计算机中，应用二进制接口描述了应用程序（或者其他类型）和操作系统之间或其他应用程序的低级接口。

EABI：嵌入式ABI。嵌入式应用二进制接口指定了文件格式、数据类型、寄存器使用、堆积组织优化和在一个嵌入式软件中的参数的标准约定。开发者使用自己的汇编语言也可以使用 EABI 作为与兼容的编译器生成的汇编语言的接口。

两者主要区别是，ABI是计算机上的，EABI是嵌入式平台上（如ARM，MIPS等）。

----


