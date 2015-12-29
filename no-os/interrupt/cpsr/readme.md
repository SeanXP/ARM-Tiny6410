interrupt - cpsr
================

以非向量中断的形式，配置中断向量表，触发ARM不同工作模式的异常，并打印出对应CPSR寄存器的值。

1. IROM启动方式下, Booting Device Region by XOM Setting(引导镜像区, 地址0x0 ~ 0x07FFFFFF)被映射到IROM地址(0x0800_0000 ~ 0x0BFF_FFFF);

2. 而中断向量表要求是在0x0~0x3FF

3. 因此，使用MMU, 将MVA(0x0 ~ 0x3FF) 映射到内存SDRAM(起始地址0x50000000)的一块区域;
