OBJECT=echo_server echo_client
CC=gcc
CROSS_CC=arm-linux-gcc
FLAGS=

TARGET:$(OBJECT)
#-------------------------------
echo_client:echo_client.c
	$(CC) $< $(FLAGS) -o $@
echo_server:echo_server.c
	$(CROSS_CC) $< $(FLAGS) -o $@
#-------------------------------


clean:
	-rm $(OBJECT)
tag:
	ctags -R .
