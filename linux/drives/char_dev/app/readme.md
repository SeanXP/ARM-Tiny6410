char device test
====

1. 加载char_dev.ko

	# insmod char_dev.ko
	# dmesg | tail


2. 创建设备节点，设备节点Major编号通过dmesg查看, 例如Major为250的情况: 

	mknod /dev/char_dev c 250 0

3. 运行测试程序

	./char_dev_test read
	./char_dev_test write "write what I want to write..."
	./char_dev_test read
	./char_dev_test ioctl clear
	./char_dev_test read
	dmesg | tail

4. 卸载程序

	# rmmod char_dev
	# dmesg | tail

5. 删除设备节点

	# rm /dev/char_dev
