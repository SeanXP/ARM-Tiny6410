# 3. 得到start.o/main.o目标文件后，执行此部分;
# ld, 链接器; 将所有的.o目标文件链接起来成为可执行文件elf;
# -Ttext 0x50000000, 配置程序的运行地址为0x50000000;程序只有在此位置才会正常运行;
# objcopy -O binary, 将elf文件以二进制数据的形式保存为bin文件;			
# objdump -D, 将elf文件反汇编为dis文件, 用于调试程序
led.bin: start.o main.o
	arm-linux-ld -Ttext 0x50000000 -o led.elf $^
	arm-linux-objcopy -O binary led.elf led.bin
	arm-linux-objdump -D led.elf > led_elf.dis

# Make后的第一步, 1. 根据.S汇编文件，通过arm-linux-gcc编译为.o目标文件;
%.o : %.S
	arm-linux-gcc -o $@ $< -c
# 2. .c编译为.o;
%.o : %.c
	arm-linux-gcc -o $@ $< -c 
clean:
	rm *.o *.elf *.bin *.dis  -rf
