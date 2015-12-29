/****************************************************************
    Copyright (C) 2015 Sean Guo. All rights reserved.
					      									  
    > File Name:         < nand.h >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2015/10/20 >
    > Last Changed: 
    > Description:		S3C6410 Nand Flash Controller - Nand Flash (K9K8G08U0E)
****************************************************************/
#ifndef __nand__
#define __nand__

// 设置NAND Flash控制器
void nand_init(void);

// Read ID, 存放在数组buffer[0] ~ buffer[5];
void nand_readID(unsigned char* buffer);

// Reset Nand Flash
void nand_reset(void);

// 等待Nand Flash Ready信号; 添加static关键字，表示为内部函数, 其他文件不可见;
static void nand_wait_idle(void);

// send address
static void nand_write_address(unsigned long address);

// 把buffer写入addr地址, size必须为页大小的整数倍(2048, 4096 ...)
void nand_write_page(unsigned char* buffer, int size, unsigned long start_addr);

// Read a page from start_addr, read (size) bytes to *buffer;
void nand_read_page(unsigned char* buffer, unsigned long start_addr, int size);

// 读一页，即2048byte
int nandll_read_page (unsigned char *buf, unsigned long addr);

#endif /* __nand__ */
