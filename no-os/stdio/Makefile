# ld, 链接器; 将所有的.o目标文件链接起来成为可执行文件elf;
# -Ttext 0x50000000, 配置程序的运行地址为0x50000000;程序只有在此位置才会正常运行;
# objcopy -O binary, 将elf文件以二进制数据的形式保存为bin文件;			
# objdump -D, 将elf文件反汇编为dis文件, 用于调试程序
# -T ddr.lds, 指定链接脚本文件
OBJECT=stdio
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump
INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -Os -fno-builtin
CPPFLAGS   	:= -nostdinc -I$(INCLUDEDIR)
export 	CC AR LD OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS 

$(OBJECT).bin: start.o clock.o sdram.o uart.o main.o lib/libc.a
	${LD} -T $(OBJECT).lds -o $(OBJECT).elf $^
	${OBJCOPY} -O binary $(OBJECT).elf $(OBJECT).bin
	${OBJDUMP} -D $(OBJECT).elf > $(OBJECT).dis
%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
clean:
	make  clean -C lib
	rm *.o *.elf *.bin *.dis  -rf
send:
	make
	cp $(OBJECT).bin ~/Downloads/1.bin
	make clean
