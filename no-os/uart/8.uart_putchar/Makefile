# ld, 链接器; 将所有的.o目标文件链接起来成为可执行文件elf;
# -Ttext 0x50000000, 配置程序的运行地址为0x50000000;程序只有在此位置才会正常运行;
# objcopy -O binary, 将elf文件以二进制数据的形式保存为bin文件;			
# objdump -D, 将elf文件反汇编为dis文件, 用于调试程序
OBJECT=uart
$(OBJECT).bin: start.o main.o clock.o uart.o
	arm-linux-ld -Ttext 0x50000000 -o $(OBJECT).elf $^
	arm-linux-objcopy -O binary $(OBJECT).elf $(OBJECT).bin
	arm-linux-objdump -D $(OBJECT).elf > $(OBJECT).dis
%.o : %.S
	arm-linux-gcc -o $@ $< -c
%.o : %.c
	arm-linux-gcc -o $@ $< -c 
#	arm-linux-gcc -o $@ $< -c -fno-builtin 
# GCC编译器提供了很多内置函数来来替换C库函数，以达到优化目的
# 使用-fno-builtin参数来关闭GCC内置函数功能。

clean:
	rm *.o *.elf *.bin *.dis  -rf
