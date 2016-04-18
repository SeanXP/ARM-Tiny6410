[FriendlyARM - Tiny6410](http://www.arm9.net/tiny6410.asp)
====

##Hardware Info

| Hardware | Description |
| :----: | :---------: |
| CPU | **SAMSUNG S3C6410A**, [ARM1176JZF-S](doc/arm-naming-rules/readme.md), run at 533Mhz, up to 667Mhz |
| Board | **Tiny6410 (v1308)** |
| SDRAM | SAMSUNG K4X2G323PD-8GD8, Mobile DDR 64Mx32(=2048Mb = **256GB** ) 200Mhz Halogen Free FBGA90 DRAM |
| FLASH | SAMSUNG K9K8G08U0E-SCB0, the same datasheet), 8G bit(= **1GB** ) Nand Flash 1Gx8 SLC |
| LEDs | LED1~4 => GPK4~7, active-low |
| Button | Reset button, M_nRESET, active-low |
| JTAG | 10 pin 2.0mm space JTAG connector |
| Expander Board | **Tiny6410SDK(TinyADK 1312B) with 7"LCD** |
| Power | Supply Voltage from 2.0V to 6V |
| Button | K1~K4 => XEINT16~19, pull-up resistor, active-low |
| Buzzer | XpwmTOUT0 (XpwmTOUT0/XCLKOUT/GPF14) |
| I2C-EEPROM | ATMLH346 (Xi2cSCL0, Xi2cSDA0) |
| USB Device | miniUSB, USB 2.0 |
| USB Host | USB A, USB 2.0 hub(x3) |

##Folder Description

| folder | description |
| :----: | :---------: |
| **arm-asm** | ARM-S3C6410 Assembler |
| **no-os** | ARM-S3C6410 Bare-Metal Program |
| **linux** | ARM-S3C6410 Linux Program |
| **doc** | ARM-S3C6410 documents |


## Tiny6410 - v1308

###crystal

| crystal | interface | description |
| :----: | :----: | :---------: |
| External Clock | XT0,XTI | Y1, 12M |
| X27MXTO | 27MXTO,27MXTI | Y2, 27M |
| USB OTG Clock | OTGTO,OTGTI | Y3, 48M |
| RTC Clock | RTCTO,RTCTI | Y4, 32.768K |

###NAND FLASH - K9K8G08U0E

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

###SDRAM - K4X2G323PD-8GD8

<http://www.samsung.com/us/business/oem-solutions/pdfs/PSG_2H_2012.pdf>

    k4x2g323pd-8gd8
    k:		samsung memort
    4:	 	dram
    x:		mobile ddr sdram
    2g:	2g, 8k/64ms
    32:	x32 (bit organization)
    3:		4 of internal banks
    p:		lvttl, 1.8v, 1.8v
    d:		5th generation
    -
    s/d: 90balls fbga

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

注意: **必须先给板子上电，然后再打开MiniTools.exe**, 否则MiniTools.exe会卡死无响应。

###[FriendlyARM Tiny6410 下载程序 - SDHC](no-os/sd-no-os/readme.md)

###交叉编译链 ARM Compilers Toolchains

[ARM Compilers](http://elinux.org/ARMCompilers)

[arm-linux-gcc-4.5.1-v6-vfp-20101103](http://www.arm9.net/download.asp)

[Download: arm-2014.05-29-arm-none-linux-gnueabi-i686-pc-linux-gnu](https://sourcery.mentor.com/GNUToolchain/package12813/public/arm-none-linux-gnueabi/arm-2014.05-29-arm-none-linux-gnueabi-i686-pc-linux-gnu.tar.bz2)
