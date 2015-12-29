Linux Device Drivers - Buttons
====

**FriendlyARM - Tiny6410 - Linux Device Drivers - 标准字符设备驱动 - Buttons**

需要手动创建设备节点: 

	# insmod buttons_dev.ko

得到分配的设备号, 假设为253, 则创建设备节点

	# mknod /dev/buttonsdev c 253 0


使用app/的应用程序进行测试，测试结果如下: 

	[root@FriendlyARM/root]# ./buttons_dev_test
	buttonsdev request_irq: 101, BUTTON0
	buttonsdev request_irq: 102, BUTTON1
	buttonsdev request_irq: 103, BUTTON2
	buttonsdev request_irq: 104, BUTTON3
	buttonsdev open().
	buttonsdev BUTTON0 interrupt.
	interrupt: button 0, p
	read key values: <p,r,r,r>
											Button 0 pressed
	buttonsdev BUTTON0 interrupt.
	interrupt: button 0, r
	read key values: <r,r,r,r>
											Button 0 release
	buttonsdev BUTTON1 interrupt.
	interrupt: button 1, p
	read key values: <r,p,r,r>
											Button 1 pressed
	buttonsdev BUTTON1 interrupt.
	interrupt: button 1, r
	read key values: <r,r,r,r>
											Button 1 release
	buttonsdev BUTTON2 interrupt.
	interrupt: button 2, p
	read key values: <r,r,p,r>
											Button 2 pressed
	buttonsdev BUTTON2 interrupt.
	interrupt: button 2, r
	read key values: <r,r,r,r>
											Button 2 release

----

	K1 -> GPN0 -> GPNCON[1:0] / GPNDAT[0] -> External interrupt Group 0, EINT0
	K2 -> GPN1 -> GPNCON[3:2] / GPNDAT[1] -> External interrupt Group 0, EINT1
	K3 -> GPN2 -> GPNCON[5:4] / GPNDAT[2] -> External interrupt Group 0, EINT2
	K4 -> GPN3 -> GPNCON[7:6] / GPNDAT[3] -> External interrupt Group 0, EINT3

