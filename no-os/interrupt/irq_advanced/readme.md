S3C6410 - VIC - IRQ interrupt
======

[https://github.com/SeanXP/ARM-Tiny6410/tree/master/interrupt/irq]与[https://github.com/SeanXP/ARM-Tiny6410/tree/master/interrupt/cpsr]
用于对比非向量/向量中断的编程。

不过irq/文件虽然实现了按键的外部中断，但是没有区分具体哪个按键，也没有进行滤波等。

此文件夹进行进一步优化;

* 使用C语言中断
* 中断程序中判断外部中断引脚，根据不同的引脚执行不同的功能
* 配置硬件滤波
* 开启MMU, 并映射用到的寄存器
