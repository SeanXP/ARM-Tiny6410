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

# 交叉编译细节
info:
	# 1. cpp预处理
	# gcc -E -o main.i main.c        # -E, 预处理后停止，不进行编译;
	# gcc -E, 等价于cpp预处理;
	arm-linux-cpp -o main.i main.c	
	# ------------------------------------------------------------------
	# 2. 编译(这里为狭义的'编译', 即高级语言编译出汇编代码)
	# gcc -S -o main.s main.i		 # -S, 预处理、编译后停止，不进行汇编;
	# gcc -S , 等价于cc1编译; 
	/home/hit413/Downloads/opt/FriendlyARM/toolschain/4.5.1/bin/../libexec/gcc/arm-none-linux-gnueabi/4.5.1/cc1  main.i -o main.s
	# ------------------------------------------------------------------
	# 3. 汇编
	# gcc -c -o main.o main.s		 # -c, 预处理、编译、汇编后停止，不进行连接；
	# gcc -c, 等价于as汇编;
	arm-linux-as -o main.o main.s
	# ------------------------------------------------------------------
	
	# 1-1. 同样, start.S需要预处理、汇编
	# 命名规范: .S汇编文件需要预处理, .s汇编文件不需要预处理;
	# arm-linux-gcc -E -o start.s start.S, # -E, 预处理
	# 这里使用交叉编译工具链的cc1, 加上arm的选项, 预处理.S汇编文件;
	/home/hit413/Downloads/opt/FriendlyARM/toolschain/4.5.1/bin/../libexec/gcc/arm-none-linux-gnueabi/4.5.1/cc1 -E -lang-asm -quiet -v -iprefix /home/hit413/Downloads/opt/FriendlyARM/toolschain/4.5.1/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/ -isysroot /home/hit413/Downloads/opt/FriendlyARM/toolschain/4.5.1/bin/../arm-none-linux-gnueabi/sys-root start.S -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp -fno-directives-only -o start.s
	# ------------------------------------------------------------------
	# 3-1. 汇编
	#/home/hit413/Downloads/opt/FriendlyARM/toolschain/4.5.1/bin/../lib/gcc/arm-none-linux-gnueabi/4.5.1/../../../../arm-none-linux-gnueabi/bin/as -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp -meabi=5 -o start.o start.s	
	arm-linux-as -o start.o start.s
	# ------------------------------------------------------------------
	# 4. 链接
	arm-linux-ld -Ttext 0x50000000 -o led.elf main.o start.o 
		

infoclean:
	rm *.i *.s 
	make clean
