so:add.c sub.c main.c
	make clean
	gcc -fpic -c add.c
	gcc -fpic -c sub.c
	gcc add.o sub.o -shared -o libtiger.so
	gcc main.c -ltiger -L. -o main_so
	file libtiger.so main_so 
a:add.c sub.c main.c
	make clean
	gcc -fpic -c add.c
	gcc -fpic -c sub.c
	ar rcs libtiger.a add.o sub.o
	gcc main.c -ltiger -L. -o main_a
	file libtiger.a main_a
clean:
	-rm *.o *.a *.so main_*
