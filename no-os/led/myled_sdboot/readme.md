myled_sdboot
====

代码与myled完全相同    
不同点：Makefile中的链接地址，不再是-Ttext 50000000, 而是-Ttext 0C000000    

因为myled是通过MiniTools.ext直接下载到SDRAM中(首地址0x5000_0000),    
而通过SD卡启动时，代码烧写在SD卡的BL1区域，复制到Stepping Stone中运行(地址0x0C00_0000);

因此，只需要在myled/的基础上，修改Makefile中链接地址为0x0C00000，程序就可以在SD卡BL1启动。

-------


实验：错误的链接地址会出现什么样的结果呢？      

###错误的链接地址

新建文件夹: test1_error_addr/         
拷贝myled_sdboot/的所有文件到此目录;        

修改链接地址为0x50000000，即一个错误的链接地址。（此代码中并未初始化SDRAM）

烧写到SD-BL1区域，运行程序。    
结果： 程序执行leds_init()函数，LED全部点亮; 但程序不执行接下来的leds_OFF_all()函数以及后面所有的函数;    
       其结果为4个LED灯一直为亮。    

1. 屏蔽main.c函数中的leds_init()函数，发现LED灯不会点亮。说明之前错误的链接地址情况下，leds_init()正常执行。

2. 新建文件夹: test2_error_addr_upgrade/, 在test1_error_addr/的基础上，修改leds.c代码，更改leds_OFF_all()函数，
不再调用需要参数的函数，直接调用宏定义。测试结果：leds_OFF_all()正常运行；


### 反汇编对比

main():
	
	50000000 <_start>:
	50000000:	e3a00207 	mov	r0, #1879048192	; 0x70000000
	50000004:	e3800013 	orr	r0, r0, #19
	50000008:	ee0f0f92 	mcr	15, 0, r0, cr15, cr2, {4}
	5000000c:	e59f0010 	ldr	r0, [pc, #16]	; 50000024 <halt+0x4>
	50000010:	e3a01000 	mov	r1, #0
	50000014:	e5801000 	str	r1, [r0]
	50000018:	e59fd008 	ldr	sp, [pc, #8]	; 50000028 <halt+0x8>
	5000001c:	eb000002 	bl	5000002c <main>

	50000020 <halt>:
	50000020:	eafffffe 	b	50000020 <halt>
	50000024:	7e004000 	cdpvc	0, 0, cr4, cr0, cr0, {0}
	50000028:	0c002000 	stceq	0, cr2, [r0], {0}

	5000002c <main>:
	5000002c:	e92d4800 	push	{fp, lr}
	50000030:	e28db004 	add	fp, sp, #4
	50000034:	e24dd008 	sub	sp, sp, #8
	50000038:	e3a03000 	mov	r3, #0
	5000003c:	e50b3008 	str	r3, [fp, #-8]
	50000040:	eb000016 	bl	500000a0 <leds_init>
	50000044:	eb000094 	bl	5000029c <leds_OFF_all>
	50000048:	e3a03000 	mov	r3, #0
	5000004c:	e1a00003 	mov	r0, r3
	50000050:	e24bd004 	sub	sp, fp, #4
	50000054:	e8bd8800 	pop	{fp, pc}


leds_init():


	500000a0 <leds_init>:
	500000a0:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
	500000a4:	e28db000 	add	fp, sp, #0
	500000a8:	e59f2034 	ldr	r2, [pc, #52]	; 500000e4 <leds_init+0x44>
	500000ac:	e59f3030 	ldr	r3, [pc, #48]	; 500000e4 <leds_init+0x44>
	500000b0:	e5933000 	ldr	r3, [r3]
	500000b4:	e1a03803 	lsl	r3, r3, #16
	500000b8:	e1a03823 	lsr	r3, r3, #16
	500000bc:	e5823000 	str	r3, [r2]
	500000c0:	e59f201c 	ldr	r2, [pc, #28]	; 500000e4 <leds_init+0x44>
	500000c4:	e59f3018 	ldr	r3, [pc, #24]	; 500000e4 <leds_init+0x44>
	500000c8:	e5933000 	ldr	r3, [r3]
	500000cc:	e3833411 	orr	r3, r3, #285212672	; 0x11000000
	500000d0:	e3833811 	orr	r3, r3, #1114112	; 0x110000
	500000d4:	e5823000 	str	r3, [r2]
	500000d8:	e28bd000 	add	sp, fp, #0
	500000dc:	e8bd0800 	pop	{fp}
	500000e0:	e12fff1e 	bx	lr
	500000e4:	7f008800 	svcvc	0x00008800


test1 - leds_OFF_all(); 不正常运行;

	5000029c <leds_OFF_all>:
	5000029c:	e92d4800 	push	{fp, lr}
	500002a0:	e28db004 	add	fp, sp, #4
	500002a4:	e24dd008 	sub	sp, sp, #8
	500002a8:	e3a03001 	mov	r3, #1
	500002ac:	e14b30b6 	strh	r3, [fp, #-6]
	500002b0:	e3a03001 	mov	r3, #1
	500002b4:	e14b30b6 	strh	r3, [fp, #-6]
	500002b8:	ea000005 	b	500002d4 <leds_OFF_all+0x38>
	500002bc:	e15b30b6 	ldrh	r3, [fp, #-6]
	500002c0:	e1a00003 	mov	r0, r3
	500002c4:	ebffffb4 	bl	5000019c <leds_OFF>
	500002c8:	e15b30b6 	ldrh	r3, [fp, #-6]
	500002cc:	e2833001 	add	r3, r3, #1
	500002d0:	e14b30b6 	strh	r3, [fp, #-6]
	500002d4:	e15b30b6 	ldrh	r3, [fp, #-6]
	500002d8:	e3530004 	cmp	r3, #4
	500002dc:	9afffff6 	bls	500002bc <leds_OFF_all+0x20>
	500002e0:	e24bd004 	sub	sp, fp, #4
	500002e4:	e8bd8800 	pop	{fp, pc}


test2 - leds_OFF_all(), 正常运行;

	
	500002b8 <leds_OFF_all>:
	500002b8:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
	500002bc:	e28db000 	add	fp, sp, #0
	500002c0:	e59f302c 	ldr	r3, [pc, #44]	; 500002f4 <leds_OFF_all+0x3c>
	500002c4:	e59f2028 	ldr	r2, [pc, #40]	; 500002f4 <leds_OFF_all+0x3c>
	500002c8:	e5922000 	ldr	r2, [r2]
	500002cc:	e3822020 	orr	r2, r2, #32
	500002d0:	e5832000 	str	r2, [r3]
	500002d4:	e59f3018 	ldr	r3, [pc, #24]	; 500002f4 <leds_OFF_all+0x3c>
	500002d8:	e59f2014 	ldr	r2, [pc, #20]	; 500002f4 <leds_OFF_all+0x3c>
	500002dc:	e5922000 	ldr	r2, [r2]
	500002e0:	e3822040 	orr	r2, r2, #64	; 0x40
	500002e4:	e5832000 	str	r2, [r3]
	500002e8:	e28bd000 	add	sp, fp, #0
	500002ec:	e8bd0800 	pop	{fp}
	500002f0:	e12fff1e 	bx	lr
	500002f4:	7f008808 	svcvc	0x00008808


test1与test2的区别, 就是leds_OFF_all()不同，一个是调用了其他带参函数，另一个为直接使用宏定义，内部没有调用函数。    
由于链接地址错误，猜想因为调用带参数的函数时返回地址或函数帧指针错误。    

b, bl, 都是短距离(32M距离)跳转指令。    
bl指令在跳转的同时，将返回地址(要运行的下一条指令的地址)拷贝到lr寄存器中。   
这些跳转都与链接地址无关，因此链接地址错误也可以执行汇编程序；    

