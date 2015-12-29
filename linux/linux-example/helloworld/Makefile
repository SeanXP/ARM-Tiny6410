OBJECT=hello
CC=arm-linux-gcc
FLAGS=

TARGET:$(OBJECT)
#-------------------------------
$(OBJECT):%:%.c
	$(CC) $< $(FLAGS) -o $@
#-------------------------------

clean:
	-rm $(OBJECT)
