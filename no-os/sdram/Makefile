# ld, 链接器; 将所有的.o目标文件链接起来成为可执行文件elf;
# -Ttext 0x50000000, 配置程序的运行地址为0x50000000;程序只有在此位置才会正常运行;
# objcopy -O binary, 将elf文件以二进制数据的形式保存为bin文件;			
# objdump -D, 将elf文件反汇编为dis文件, 用于调试程序
# -T ddr.lds, 指定链接脚本文件
OBJECT=sdram
$(OBJECT).bin: start.o main.o clock.o uart.o sdram.o
	arm-linux-ld -T sdram.lds -o $(OBJECT).elf $^
	arm-linux-objcopy -O binary $(OBJECT).elf $(OBJECT).bin
	arm-linux-objdump -D $(OBJECT).elf > $(OBJECT).dis
%.o : %.S
	arm-linux-gcc -o $@ $< -c
%.o : %.c
	#arm-linux-gcc -o $@ $< -c 
	arm-linux-gcc -g -c -O2 -o $@ $^
clean:
	rm *.o *.elf *.bin *.dis  -rf

test1: 
	arm-linux-gcc -o start.o start.S -c
	arm-linux-gcc -o clock.o clock.c -c
	arm-linux-gcc -g -c -O2 -o main.o main.c	# must -O2 !!!
	arm-linux-gcc -g -c -O2 -o uart.o uart.c	# must -O2 !!!
	arm-linux-gcc -g -c -O2 -o sdram.o sdram.c  # must -O2 !!!
	arm-linux-ld -T sdram.lds -o sdram.elf start.o main.o clock.o uart.o sdram.o
	arm-linux-objcopy -O binary sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis
test2:
	arm-linux-gcc -o start.o start.S -c
	arm-linux-gcc -o clock.o clock.c -c
	#arm-linux-gcc -o main.o main.c -c			# 没有-O2优化选项，导致程序无法正常工作!
	arm-linux-gcc -g -c -o main.o main.c		# 删掉-O2优选选项，发现串口无法输入
	#arm-linux-gcc -o uart.o uart.c -c
	arm-linux-gcc -g -c -O2 -o uart.o uart.c
	#arm-linux-gcc -o sdram.o sdram.c -c
	arm-linux-gcc -g -c -O2 -o sdram.o sdram.c
	arm-linux-ld -T sdram.lds -o sdram.elf start.o main.o clock.o uart.o sdram.o
	arm-linux-objcopy -O binary sdram.elf sdram.bin
	arm-linux-objdump -D sdram.elf > sdram.dis
