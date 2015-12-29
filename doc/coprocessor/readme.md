Arm - 协处理器 Coprocessor
====

###协处理器CP15

| Register        | Read          | Write |
| :-------------: |:-------------| -----:| 
| C0      | ID Code (ID编码) | Unpredictable  |
| C0      | Catch type(Catch类型)      |  Unpredictable |
| C1 | Control (控制位)     |   Control |
| C2|Translation table base(地址转换表基地址)|Translation table base|
| C3|Domain access control(域访问控制位)|Domain access control|
| C4|Unpredictable(保留)|Unpredictable|
| C5|Fault status(内存失效状态)|Fault status|
| C6|Fault address(内存失效地址)|Fault address|
| C7|Unpredictable|Cache operations(高速缓存与写缓存控制)|
| C8|Unpredictable|TLB operations(TLB控制)|
| C9|Cache lockdown(高速缓存锁定)|Cache lockdown|
| C10|TLB lock down(TLB锁定)|TLB lock down|
| C11|Unpredictable(保留)|Unpredictable|
| C12|Unpredictable(保留)|Unpredictable|
| C13|Process ID(进程标识符PID)|Process ID|
| C14|Unpredictable(保留)|Unpredictable|
| C15|Test configuration(因不同设计而异)|Test configuration|

###协处理器指令

ARM 微处理器可支持多达 16 个协处理器，用于各种协处理操作，在程序执行的过程中，每个协处理器只执行针对自身的协处理指令，忽略 ARM 处理器和其他协处理器的指令。

ARM 的协处理器指令主要用于 ARM 处理器初始化 ARM 协处理器的数据处理操作，以及在ARM 处理器的寄存器和协处理器的寄存器之间传送数据，和在 ARM协处理器的寄存器和存储器之间传送数据。 ARM 协处理器指令包括以下 5 条：

* CDP 协处理器数操作指令
* LDC 协处理器数据加载指令
* STC 协处理器数据存储指令
* MCR ARM 处理器寄存器到协处理器寄存器的数据传送指令
* MRC 协处理器寄存器到ARM 处理器寄存器的数据传送指令

1. CDP 指令     
	CDP 指令的格式为：
			
		CDP{条件} 协处理器编码，协处理器操作码1，目的寄存器，源寄存器1，源寄存器2，协处理器操作码2。
	CDP 指令用于ARM 处理器通知ARM 协处理器执行特定的操作,若协处理器不能成功完成特定的操作，则产生未定义指令异常。其中协处理器操作码1 和协处理器操作码2 为协处理器将要执行的操作，目的寄存器和源寄存器均为协处理器的寄存器，指令不涉及ARM 处理器的寄存器和存储器。    
	指令示例：     
		
		CDP P3, 2, C12, C10, C3, 4 //该指令完成协处理器 P3 的初始化
2. LDC 指令      

	LDC 指令的格式为：     

		LDC{条件}{L} 协处理器编码, 目的寄存器, [源寄存器]

	LDC 指令用于将源寄存器所指向的存储器中的字数据传送到目的寄存器中，若协处理器不能成功完成传送操作，则产生未定义指令异常。其中，{L}选项表示指令为长读取操作，如用于双精度数据的传输。        

	指令示例：      

		LDC P3, C4, [R0]  //将 ARM 处理器的寄存器 R0 所指向的存储器中的字数据传送到协处理器 P3 的寄存器 C4 中。


3. STC 指令

	STC 指令的格式为：

		STC{条件}{L} 协处理器编码, 源寄存器, [目的寄存器]

	STC 指令用于将源寄存器中的字数据传送到目的寄存器所指向的存储器中，若协处理器不能成功完成传送操作，则产生未定义指令异常。其中，{L}选项表示指令为长读取操作，如用于双精度数据的传输。

	指令示例：

		STC P3, C4, [R0]  将协处理器 P3 的寄存器 C4 中的字数据传送到 ARM 处理器的寄存器R0 所指向的存储器中。
		
4. **MCR 指令**

	MCR 指令的格式为：

		MCR{条件} 协处理器编码，协处理器操作码1，源寄存器，目的寄存器1，目的寄存器2，协处理器操作码2。

	MCR 指令用于将ARM 处理器寄存器中的数据传送到协处理器寄存器中,若协处理器不能成功完成操作，则产生未定义指令异常。其中协处理器操作码1 和协处理器操作码2 为协处理器将要执行的操作，源寄存器为ARM 处理器的寄存器，目的寄存器1 和目的寄存器2 均为协处理器的寄存器。

	指令示例：

		MCR P3, 3, R0, C4, C5, 6 //该指令将 ARM 处理器寄存器 R0 中的数据传送到协处理器 P3 的寄存器 C4 和 C5 中。
		
5. **MRC 指令**

	MRC 指令的格式为：

		MRC{条件} 协处理器编码，协处理器操作码1，目的寄存器，源寄存器1，源寄存器2，协处理器操作码2。

	MRC 指令用于将协处理器寄存器中的数据传送到ARM 处理器寄存器中,若协处理器不能成功完成操作，则产生未定义指令异常。其中协处理器操作码1 和协处理器操作码2 为协处理器将要执行的操作，目的寄存器为ARM 处理器的寄存器，源寄存器1 和源寄存器2 均为协处理器的寄存器。

	指令示例：

		MRC P3, 3, R0, C4, C5, 6  //该指令将协处理器 P3 的寄存器中的数据传送到 ARM 处理器寄存器中.
		
----

参考: <http://6xudonghai.blog.163.com/blog/static/336406292008724103317304/>

