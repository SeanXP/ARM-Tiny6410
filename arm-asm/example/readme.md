示例程序
===========

开始先执行main.S中的main函数, 传递参数给三个寄存器, 然后调用print函数(print.c中实现的函数);

print函数使用了printf()函数，这里包含stdio.h头文件, test在arm板子执行时，会调用arm板子的stdio库。
