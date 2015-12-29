char device - leds
====

使用标志的char device驱动程序来驱动LED灯;

需要自己手动创建节点。


也可以利用udev方法来自动生成节点, 参考char_udev_leds\目录;
char_udev_leds.c与char_leds.c的区别，在于char_udev_leds.c在init()注册驱动的同时，使用udev方法创建节点;
