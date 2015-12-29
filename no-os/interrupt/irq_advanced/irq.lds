SECTIONS 
{
	/* 指定定位器符号 */
	/* Stepping Stone中运行*/
	/*. = 0x0C000000;*/

	/* 内存中运行 */
    . = 0x50000000;

	/* 先存放.text 代码段, 再存放.data 数据段, 最后存放 .bss 数据段 */
	.text : {
			start.o
			clock.o
			sdram.o
			mmu.o
			main.o
			uart.o
			irq.o
			key.o
			leds.o
			* (.text)
	}

    . = ALIGN(4);
    .rodata :
    {
    	* (.rodata)
    }

    /*. = ALIGN(4);*/
	.data : {
		* (.data)
	}

    /*. = ALIGN(4);*/
    bss_start = .;
	.bss : {
		* (.bss)    
	}
    bss_end = .;
}
