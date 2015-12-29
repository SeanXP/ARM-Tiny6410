link.bin: start.o main.o clock.o uart.o
	arm-linux-ld -T  link.lds -o link.elf start.o main.o clock.o uart.o
	arm-linux-objcopy -O binary link.elf link.bin
	arm-linux-objdump -D link.elf > link.dis

%.o : %.S
	arm-linux-gcc -o $@ $< -c

# -fno-builtin, 不使用C语言的内建函数; 这里uart.h中定义了putchar()函数，与C语言内建函数冲突;
%.o : %.c
	arm-linux-gcc -o $@ $< -c -fno-builtin 

clean:
	rm *.o *.elf *.bin *.dis -f
