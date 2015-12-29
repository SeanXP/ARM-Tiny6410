Nameing Rules
====

**ARM是Advanced RISC Machines的缩写，是典型的RISC(Reduced Instruction Set Computer)架构的CPU**。    
ARM的版本控制的命名规则分成两类。

* 一类是基于ARM 架构的版本命名规则，它是一种架构设计的总和；
* 另一类是基于某ARM架构版本的系列处理器的命名规则。

###ARM架构命名规则

	| ARMv | n | variants | x（variants） | 
	
ARM架构的命名由四部分组成：   

* ARMv字符串，这部分固定不变。
* n，ARM指令集版本号，ARM架构版本发布了7个系列，所以n=[1:7]。其中最新的版本是第7版，Cortex系列CPU采用该版本。
* variants，变种，通常用大写字母来表示对一类指令或者指令集，变种就是这些大写字母的连写。   
	常见的变种有：

	* T，Thumb指令集
	* M，长乘法指令
	* E，增强型DSP指令
	* J，Java虚拟机Jazelle指令集
	* P，LDRD, MCRR, MRRC, PLD和STRD指令   

	例如，ARMv5TxP表示ARM指令集版本为5，支持Thumb指令集，不支持LDRD, MCRR, MRRC, PLD和STRD指令。    
* x（variants），不支持x后指定的变种。

###ARM处理器命名规则

ARM处理器的命名规则如下：

	ARM{x}{y}{z}{T}{D}{M}{I}{E}{J}{F}{-S}
	
各组成部分解释如下：

    * x，处理器系列
    * y，存储管理/保护单元
    * z，cache
    * T，支持Thumb指令集
    * D，支持片上调试
    * M，支持快速乘法器
    * I，支持Embedded ICE，支持嵌入式跟踪调试
    * E，支持增强型DSP指令
    * J，支持Jazelle
    * F，具备向量浮点单元VFP
    * -S， 可综合版本

ARM使用一种基于数字的命名法。在早期（1990s），还在数字后面添加字母后缀，用来进一步明细该处理器支持的特性。    
就拿ARM7TDMI来说，T代表Thumb指令集，D是说支持JTAG调试(Debugging)，M意指快速乘法器，I则对应一个嵌入式ICE模块。后来，这4项基本功能成了任何新产品的标配，于是就不再使用这4个后缀相当于默许了。    
但是新的后缀不断加入，包括定义存储器接口的，定义高速缓存的，以及定义"紧耦合存储器（TCM）"的，于是形成了新一套命名法，这套命名法一直使用至今。比如ARM1176JZF-S，它实际上默认就支持TDMI功能，除此之外还支持JZF。

	Friendly Tiny6410 
	ARM1176JZF-S:
		TDMI: T代表Thumb指令集，D是说支持JTAG调试(Debugging)，M意指快速乘法器，I则对应一个嵌入式ICE模块。
		J: 支持Jazelle
		z，cache
		F，具备向量浮点单元VFP
		-S， 可综合版本
		
	ARM1176JZ(F)-S基于ARMv6KZ 在 ARM1136EJ(F)-S 基础上增加MMU、TrustZone。


基于ARMv7架构的ARM处理器不再沿用过去的数字命名方式，而是冠以Cortex前缀，基于ARMv7A的处理器成为Cortex-A系列，基于ARMv7R的处理器成为Cortex-R系列，基于ARMv7M的处理器成为Cortex-M系列，

