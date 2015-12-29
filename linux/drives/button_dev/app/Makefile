OBJECT=buttons_dev_test
CC=arm-linux-gcc
FLAGS=

TARGET:$(OBJECT)
#-------------------------------
$(OBJECT):%:%.c
	$(CC) $< $(FLAGS) -o $@
#-------------------------------

clean:
	-rm $(OBJECT)
send:
	make
	mv $(OBJECT) /var/nfsroot/rootfs/root/
	make clean
