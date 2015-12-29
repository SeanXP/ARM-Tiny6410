FriendlyARM - Tiny6410
====


| Hardware | description |
| :----: | :---------: |
| CPU | **SAMSUNG S3C6410**, run at 533Mhz, up to 667Mhz |
| Board | **Tiny6410 (v1308)** |
| Expander Board | **Tiny6410SDK(TinyADK 1312B) with 7"LCD** | 

[FriendlyARM - Tiny6410](http://www.arm9.net/tiny6410.asp)

----------

| folder | description |
| :----: | :---------: |
| **arm-asm** | ARM-S3C6410 Assembler |
| **no-os** | ARM-S3C6410 Bare-Metal Program |
| **linux** | ARM-S3C6410 Linux Program |
| **doc** | ARM-S3C6410 documents | 


----------


##核心板: Tiny6410 - v1308
	SDRAM:		SAMSUNG K4X2G323PD-8GD8
				Mobile DDR 64Mx32 200Mhz Halogen Free FBGA90 DRAM
				64Mbx32 = 2048Mb = 2Gb = 256MB
	
	NAND FLASH: SAMSUNG K9K8G08U0E-SCB0, 8G bit Nand Flash 1Gx8 SLC 
				 
	LED:		LED1 ~ LED4 (nLED_1 ~ nLED_4), 低电平点亮LED.
					LED1 - GPK4
					LED2 - GPK5
					LED3 - GPK6
					LED4 - GPK7
					
	JTAG:		10 pin 2.0mm space Jtag connector
	Button:		Reset button, 连接M_nRESET接口, 低电平有效;
	Power:		Supply Voltage from 2.0V to 6V

	Crystal:	Y1 - 12M - ( XTO , XTI )				- 外部时钟	
				Y2 - 27M - ( 27MXTO , 27MXTI )			- X27MXTO
				Y3 - 48M - ( OTGTO , OTGTI )			- USB OTG 时钟
				Y4 - 32.768K - ( RTCTO , RTCTI ) 		- RTC 时钟
		
		

####NAND FLASH:
SAMSUNG K9K8G08U0E-SCB0, 8G bit Nand Flash,1Gx8 SLC   
K9F4G08U0E , K9K8G08U0E, 同一个芯片手册;

型号说明：
<http://www.elnec.com/device/Samsung/K9F4G08U0E+%5BTSOP48%5D/>

	- K = Memory
	- 9 = NAND Flash
	- F = SLC Normal ; K = SLC Die Normal, SLC Die stack;
	- Density: 4G = 4Gbit, 8G = 8Gbit;
	- Technology: 0 = Normal (x8)
		- Organization: 8 = x8
		- Supply Voltage: U = 2.7V to 3.6V
		- Mode: 0 = Normal
		- Generation: E = 6th generation


####SDRAM - K4X2G323PD-8GD8
<http://www.samsung.com/us/business/oem-solutions/pdfs/PSG_2H_2012.pdf> 

	K4X2G323PD-8GD8 
	K:		SAMSUNG Memort
	4:	 	DRAM
	X:		Mobile DDR SDRAM
	2G:	2G, 8K/64ms
	32:	x32 (bit organization)		3:		4 of Internal Banks
	P:		LVTTL, 1.8V, 1.8V
	D:		5th Generation
	
	S/D: 90balls FBGA

======
###扩展底层板: Tiny6410SDK 增强版 - TinyADK 1312B
	
	Button:		K1 - XEINT16
				K2 - XEINT17
				K3 - XEINT18
				K4 - XEINT19
				Button IO接上拉电阻，按下按键后IO为拉为低电平;
	Buzzer:		XpwmTOUT0 (XpwmTOUT0/XCLKOUT/GPF14)
	I2C-EEPROM:	ATMLH346
				Xi2cSCL0, Xi2cSDA0
				
				
----



###FriendlyARM Tiny6410 下载程序 - MiniTools.exe


1. 底层板S2选择NAND启动方式

2. 按住K1按钮，打开电源开关S1

3. 此时若串口连接COM0到PC，配置波特率为115200，就可以看到串口输出消息; 不进行串口连接，也可以在液晶屏上看到同样的消息;

4. 输出消息:

		Booting from NAND

		Superboot6410
		Ver: 141011
		CPU: S3C6410 533MHz
		RAM: 256MB
		NAND: 1GB(SLC) ID:ECD35195
		Touch Device: 1-wire
		LCD Type: S70(Auto)

		USB Mode: Waiting...
		Hello USB Loop

5. 底层板通过USB与PC连接，PC识别USB设备。打开MiniTools.exe软件，识别到ARM开发板;

6. 同时串口输出消息: 
	
		USB Mode: Connected

7. 选择不同模式，下载程序;

注意：**必须先给板子上电，然后在打开MiniTools.exe**, 否则MiniTools.exe会卡死无响应。

###FriendlyARM Tiny6410 下载程序 - SDHC

[SD-Boot](https://github.com/SeanXP/ARM-Tiny6410/tree/master/sd-no-os)

------


