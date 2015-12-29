反汇编解析
==================
汇编程序start.S

	.global _start						

	_start:
		ldr r0, halt
		adr r0, halt
		ldr r0, =halt
		nop

	halt:
		// arm汇编指令: b, 跳转到对应代码函数段;
		b halt				// 死循环 
		nop

-------------
Makefile文件部分内容:

	arm-linux-ld -Ttext 0x50000000 -o $(OBJECT).elf $^

	
本程序没有.lds链接脚本文件，因此按照默认链接顺序。   
Makefile文件在链接时, '-Ttext 0x50000000' 指定了代码段的运行地址, 但其他段(eg. '.data', '.bss')没有指定，则按照默认规则;


------------
可执行程序start_elf的反汇编文件start_elf.dis:

1. 由于Makefile中指定了'.text'代码段的运行地址，因此可以看到代码段的开头`50000000 <_start>:`; 
2. 反汇编文件的格式: 运行地址, 机器码, 由机器码反汇编出来的汇编语句, 汇编语句中操作数; 
3. 因为只指定了'.text'代码段, 可以看到section .text的地址从0x50000000开始, 其中50000000 <_start>, 5000000c <halt>;
4. 其他段未指定运行地址, 其链接地址在镜像文件中连续，但运行地址就为链接器的默认指定, 例如00000000 <.ARM.attributes>;
5. adr是小范围的地址读取伪指令, ldr是大范围的读取地址伪指令;
6. 执行_start第一行汇编语句'ldr r0, halt', 大范围读取地址伪指令, 寄存器间接寻址, 读取halt标号(halt)对应的内存地址(5000000c <halt>)的值(eafffffe)。
执行第一行汇编时, PC在取值时已叠加为下一条指令地址PC=0x50000004; 汇编语句中的'halt'为标号，其代表的物理内存地址为(5000000c <halt>);
指定此汇编语句时，汇编程序计算相对于 PC 的偏移量，并生成相对于 PC的前索引的指令：ldr r0, [pc, #8]。
执行指令后，r0 = eafffffe。标号halt对应内存地址(50000010)的存储数据;
7. 执行_start的第二行汇编语句'adr r0, halt', 小范围读取地址伪指令, 总是会被汇编程序汇编为一个指令, 
汇编程序尝试产生单个 ADD 或 SUB 指令来装载该地址。如果不能在一个指令中构造该地址,则生成一个错误，并且汇编失败。
表示: 读取halt标号对应的地址到r0; 被翻译为'add r0, pc, #4', 其结果为r0=0x50000010;
adr伪指令产生的是依赖位置的代码, 将该代码与标号halt的相对位置不变的情况下移动(例如将整个代码直接移动到另外的内存空间中)，不影响adr伪指定的执行；
因为已经在汇编过程中计算好与标号的相对位置。
8. 执行_start的第三行汇编语句'ldr r0, =halt'; 汇编程序将相对程序的标号表达式 label-expr 的值放在一个文字池中，并生成一个相对程序的 LDR 指令来从文字池中装载该值;
对应文字池的地址及值: '50000018:   50000010', 在halt段结束(整个程序结束)的后面。
在链接时，就要将'ldr r0, =halt'转为一条ldr汇编指令并生成一条文字池数据。链接时要计算标号的绝对地址(50000010)并存放在文字池中；
因此，不管该汇编语句在哪里执行，与halt标号的相对位置是否改变，其结果都是固定的，跳转到一个生成的文字池，得到其数据。
转换为汇编指令为'ldr r0, [pc, #8]', 其结果为r0读取文字池(地址为50000018)的数据50000010;
宏观上看，及'ldr r0, =halt'表示获取halt标号的运行内存地址; 可以利用此语句实现PC的跳转;
usage-1: 'ldr r0, =_main' 设置r0为标号_main的运行地址; 
usage-2: 'ldr r0, =1024', 设置r0为1024; (最好不要用立即数)

	start.elf:     file format elf32-littlearm


	Disassembly of section .text:

	50000000 <_start>:
	50000000:	e59f0008 	ldr	r0, [pc, #8]	; 50000010 <halt>
	50000004:	e28f0004 	add	r0, pc, #4
	50000008:	e59f0008 	ldr	r0, [pc, #8]	; 50000018 <halt+0x8>
	5000000c:	e320f000 	nop	{0}

	50000010 <halt>:
	50000010:	eafffffe 	b	50000010 <halt>
	50000014:	e320f000 	nop	{0}
	50000018:	50000010 	andpl	r0, r0, r0, lsl r0

	Disassembly of section .ARM.attributes:

	00000000 <.ARM.attributes>:
	   0:	00002541 	andeq	r2, r0, r1, asr #10
	   4:	61656100 	cmnvs	r5, r0, lsl #2
	   8:	01006962 	tsteq	r0, r2, ror #18
	   c:	0000001b 	andeq	r0, r0, fp, lsl r0
	  10:	4d524105 	ldfmie	f4, [r2, #-20]	; 0xffffffec
	  14:	36373131 	undefined instruction 0x36373131
	  18:	2d465a4a 	vstrcs	s11, [r6, #-296]	; 0xfffffed8
	  1c:	09060053 	stmdbeq	r6, {r0, r1, r4, r6}
	  20:	01090108 	tsteq	r9, r8, lsl #2
	  24:	Address 0x00000024 is out of bounds.
