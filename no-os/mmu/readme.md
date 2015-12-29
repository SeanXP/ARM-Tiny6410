MMU-LED-C
=====

MMU配置(C语言版)

虚拟地址映射（段页表映射，每个条目对应1MB空间）：  
  
* 配置虚拟地址0xC000_0000对应0x5000_0000;  
0x5000_0000 ~ 0x5010_0000 , 1M Bytes, ==>  0xC000_0000 ~ 0xC010_0000; 

* 配置虚拟地址0x0000_0000对应0x0000_0000;

* 配置虚拟地址0x0000_0000对应0x0000_0000;    
0x7F00_0000 ~ 0x7F10_0000 , 1M Bytes, ==> 0x1000_0000 ~ 0x1010_0000;


其中0x5000_0000为DRAM存储空间地址(0x5000_0000 ~ 0x5FFF_FFFF);      
控制LED灯的寄存器GPKCON0(0x7F008800)/GPKDAT(0x7F008808)   

程序链接地址设为0xC000_0000(等价物理地址0x5000_0000)。将程序烧写至SD卡BL1区域，并切换为SDBOOT启动模式。

ARM板子上电，OM寄存器决定为IROM启动方式，GPN寄存器决定Bood Device为SD。CPU先执行IROM的厂家固化代码，然后从SD卡的BL1区域（8K）读取到Stepping Stone中（对应物理地址为0x0000_0000）,开始执行代码（Start.S）。
最后运行main.c，通过虚拟地址0x1000_8800和0x1000_8808来控制LED灯；

程序运行速度较慢，需要一段时间后才能看到main函数的运行效果。板子上电一段时间以后，led灯开始按照二进制加法规律进行闪烁。



Tiny6410 Memory Map参考：   
<https://github.com/SeanXP/ARM-Tiny6410/tree/master/doc/MemoryMap>

----

###MMU
* 虚拟地址到物理地址的转换（即虚拟内存管理）
* 内存保护(硬件机制的内存访问权限检查)
* 中央处理器高速缓存(CPU Cache)的控制    

重点就在于地址映射:**页表的结构与建立、映射的过程**

现代的多用户多线程操作系统通过MMU使得各个用户进程都拥有自己独立的地址空间：   

* 地址映射：使得各进程拥有“看起来”一样的地址空间
* 内存保护：保护每个线程所用实际内存空间不被其他进程破坏。

<https://en.wikipedia.org/wiki/Memory_management_unit>    
<https://zh.wikipedia.org/wiki/内存管理单元>



	内存管理单元（英语：memory management unit，缩写为MMU），有时称作分页内存管理单元（英语：paged memory management unit，缩写为PMMU）。
	它是一种负责处理中央处理器（CPU）的内存访问请求的计算机硬件。它的功能包括虚拟地址到物理地址的转换（即虚拟内存管理）、内存保护、中央处理器高速缓存的控制，在较为简单的计算机体系结构中，负责总线的仲裁以及存储体切换（bank switching，尤其是在8位的系统上）。
	
	现代的内存管理单元是以页的方式，分割虚拟地址空间（处理器使用的地址范围）的；
	页的大小是2的n次方，通常为几KB。地址尾部的n位（页大小的2的次方数）作为页内的偏移量保持不变。其余的地址位（address）为（虚拟）页号。
	内存管理单元通常借助一种叫做轉譯旁觀緩衝區（Translation Lookaside Buffer，缩写为TLB）的相联高速缓存（associative cache）来将虚拟页号转换为物理页号。当后备缓冲器中没有转换记录时，则使用一种较慢的机制，其中包括专用硬件（hardware-specific）的数据结构（Data structure）或软件辅助手段。这个数据结构称为分页表，页表中的数据就叫做页表项（page table entry，缩写为PTE）。物理页号结合页偏移量便提供出了完整的物理地址。

	页表或转换后备缓冲器中数据项包括的信息有：
		一、“脏位”（页面重写标志位，dirty bit）——表示该页是否被写过。
		二、“存取位”（accessed bit）——表示该页最后使用于何时，以便于最近最少使用页面置换算法（least recently used page replacement algorithm）的实现。
		三、哪种进程可以读写该页的信息，例如用户模式（user mode）进程还是特权模式（supervisor mode）进程。
		四、该页是否应被高速缓冲的信息。

	有时，TLB或PTE会禁止对虚拟页的访问，这可能是因为没有物理随机存取存储器（random access memory）与虚拟页相关联。如果是这种情况，MMU将向CPU发出页错误（page fault）的信号。操作系统operating system）将进行处理，也许会尝试寻找RAM的空白帧，同时建立一个新的PTE将之映射到所请求的虚拟地址。如果没有空闲的RAM，可能必须关闭一个已经存在的页面，使用一些替换算法，将之保存到磁盘中（这被称之为页面调度（paging）。在一些MMU中，PTEs或者TLB也存在一些缺点，在这样的情况下操作系统将必须释放空间以供新的映射。
	
	

MMU参考：  

* 《嵌入式Linux应用开发》 - 韦东山 编著      

* [嵌入式Linux学习笔记(四)-内存管理单元mmu](http://blog.chinaunix.net/attachment/attach/22/69/53/862269538680dd6573653ca251f94af0e783e58753.pdf)   

* [ARM MMU工作原理剖析](http://blog.chinaunix.net/uid-20698426-id-136197.html)


----


