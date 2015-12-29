OBJECT=start
$(OBJECT).bin: start.o
	arm-linux-ld -Ttext 0x50000000 -o $(OBJECT).elf $^
	arm-linux-objcopy -O binary $(OBJECT).elf $(OBJECT).bin
	arm-linux-objdump -D $(OBJECT).elf > $(OBJECT)_elf.dis
%.o : %.S
	arm-linux-gcc -o $@ $< -c
clean:
	rm *.o *.elf *.bin *.dis  -rf
