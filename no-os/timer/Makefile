# ld, 链接器; 将所有的.o目标文件链接起来成为可执行文件elf;
# -Ttext 0x50000000, 配置程序的运行地址为0x50000000;程序只有在此位置才会正常运行;
# objcopy -O binary, 将elf文件以二进制数据的形式保存为bin文件;			
# objdump -D, 将elf文件反汇编为dis文件, 用于调试程序
# -T ddr.lds, 指定链接脚本文件
OBJECT=timer
$(OBJECT).bin: start.o main.o clock.o uart.o sdram.o mmu.o irq.o key.o leds.o timer.o
	arm-linux-ld -T $(OBJECT).lds -o $(OBJECT).elf $^
	arm-linux-objcopy -O binary $(OBJECT).elf $(OBJECT).bin
	arm-linux-objdump -D $(OBJECT).elf > $(OBJECT).dis
%.o : %.S
	arm-linux-gcc -o $@ $< -c
%.o : %.c
	arm-linux-gcc -o $@ $< -c 
clean:
	rm *.o *.elf *.bin *.dis  -rf
send:
	make
	cp $(OBJECT).bin ~/Downloads/1.bin
	make clean
