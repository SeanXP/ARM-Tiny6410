OBJECT=leds
$(OBJECT).bin: start.o main.o leds.o
	arm-linux-ld -Ttext 0x50000000 -o $(OBJECT).elf $^
	arm-linux-objcopy -O binary $(OBJECT).elf $(OBJECT).bin
	arm-linux-objdump -D $(OBJECT).elf > $(OBJECT)_elf.dis
%.o : %.S
	arm-linux-gcc -o $@ $< -c
%.o : %.c
	arm-linux-gcc -nostdlib -o $@ $< -c
clean:
	rm *.o *.a *.so *.elf *.bin *.dis *.i *.s -rf
send:
	make
	mv $(OBJECT).bin ~/Downloads/
gcc:
	# 1. pre-process, 	gcc -E
	arm-linux-gcc -E leds.c -o leds.i
	arm-linux-gcc -E main.c -o main.i
	# 2. compile, 		gcc -S
	arm-linux-gcc -S -nostdlib leds.i -o leds.s
	arm-linux-gcc -S -nostdlib main.i -o main.s
	# 3. assemble, 		gcc -c
	arm-linux-gcc -c leds.s -o leds.o
	arm-linux-gcc -c main.s -o main.o
	arm-linux-gcc -c start.S -o start.o
ld:
	make gcc
	# 4. ld
	arm-linux-ld -Ttext 0x50000000 -o leds.elf start.o main.o leds.o
objcopy:
	make ld
	# *. objcopy
	arm-linux-objcopy -O binary leds.elf leds.bin
	file leds.elf leds.bin
objdump:
	make ld
	# *. objdump
	arm-linux-objdump -D leds.elf > leds_elf.dis
ar:
	make gcc
	# *. ar, create static lib
	arm-linux-ar -rv libleds.a leds.o
	file libleds.a
	# *. ld after ar ( using libleds.a, not leds.o )
	#arm-linux-ld -Ttext 0x50000000 start.o main.o libleds.a -o leds.elf
	arm-linux-ld -Ttext 0x50000000 start.o main.o -L. -lleds -o leds.elf
share:
	make gcc
	# create dynamic lib, 'gcc -shared'
	arm-linux-gcc -shared leds.o -o libleds.so
	file libleds.so
	# ld ( using libleds.so, not leds.o )
	arm-linux-ld -Ttext 0x50000000 start.o main.o -L. -lleds -o leds.elf
readelf:
	make ld
	arm-linux-gcc -shared leds.o -o libleds.so
	# readelf
	arm-linux-readelf -h main.o | grep Type
	arm-linux-readelf -h libleds.so | grep Type
	arm-linux-readelf -h leds.elf | grep Type
	# elf format: 1. REL (Relocatable file) 2. DYN (Shared object file) 3. EXEC (Executable file)
	file main.o libleds.so leds.elf
nm:
	make ld
	arm-linux-nm leds.o main.o leds.elf
strings:
	make ld
	arm-linux-strings main.o
strip:
	make ld
	# strip, remove symbol information
	arm-linux-strip leds.elf -o leds_stripped.elf
	ls -l *.elf
	file *.elf
	arm-linux-nm leds_stripped.elf
