OBJECT=leds_dev_test
CC=arm-linux-gcc
FLAGS=

TARGET:$(OBJECT)
#-------------------------------
$(OBJECT):%:%.c
	$(CC) $< $(FLAGS) -o $@
#-------------------------------

clean:
	-rm $(OBJECT)
