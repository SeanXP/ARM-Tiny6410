SECTIONS
{
	/* 指定定位器符号, 即初始运行地址为0x1000; */
	. = 0x1000;

	/* 先存放.text 代码段, 再存放.data 数据段, 最后存放 .bss 数据段 */
	.text : {
		start.o
		* (.text)
	}
    		
	.data : {
		* (.data)
	}
	
	bss_start = .; 
	.bss : {
		* (.bss)
	}
	bss_end  = .;	
}
