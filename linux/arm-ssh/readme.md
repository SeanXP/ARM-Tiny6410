移植SSH到ARM Linux
====


#./make.sh
执行脚本，自动下载、配置、交叉编译、安装到一个目录，只需要将文件拷贝到对应ARM开发板即可。

----

#具体移植步骤

参考博客：[移植SSH到ARM Linux开发板成功（标准步骤）](http://blog.sina.com.cn/s/blog_7f2a9a390100xws4.html)

###1. 下载源代码(需梯子)

- openssh  http://www.openssh.com/portable.html
- openssl  http://www.openssl.org/source
- zlib     http://www.zlib.net/

注意: 下面的下载地址可能会由于更新而失效;
* `wget http://mirrors.nycbug.org/pub/OpenBSD/OpenSSH/portable/openssh-7.1p1.tar.gz`
* `wget https://www.openssl.org/source/openssl-1.0.2f.tar.gz`
* `wget http://zlib.net/zlib-1.2.8.tar.gz`

###2. 创建工作目录:

	# mkdir /arm-ssh/
	# cd /arm-ssh/

	# mkdir compressed install source 
	
* compressed 用于存放源码包    
* install    软件安装目录    
* source    源码包解压目录   

###3.  拷贝安装包到compressed/目录，并解压

	# cd /arm-ssh/compressed/
	# tar zxvf zlib-1.2.3.tar.gz  -C  ../source
	# tar zxvf openssl-0.9.8e.tar.gz  -C  ../source
	# tar zxvf openssh-4.6p1.tar.gz  –C ../source

###4. 交叉编译zlib

	# cd  /arm-ssh/source/zlib-1.2.3
	# ./configure --prefix=/arm-ssh/install/zlib-1.2.3
	# vim Makefile
		CC=arm-none-linux-gnueabi-gcc
		AR=arm-none-linux-gnueabi-ar
		CPP =arm-none-linux-gnueabi-gcc -E
		LDSHARED=arm-none-linux-gnueabi-gcc
	# make
	# make install

###5. 交叉编译openssl

	# cd  /arm-ssh/source/openssl-0.9.8e
	# ./Configure --prefix=/arm-ssh/install/openssl-0.9.8e  os/compiler:arm-none-linux-gnueabi-gcc
	# make
	# make install

###6. 交叉编译openssh

	# cd  /arm-ssh/source/openssh-4.6p1
	# ./configure --host=arm-none-linux-gnueabi --with-libs --with-zlib=/arm-ssh/install/zlib-1.2.3 --with-ssl-dir=/arm-ssh/install/openssl-0.9.8e --disable-etc-default-login CC=arm-none-linux-gnueabi-gcc AR=arm-none-linux-gnueabi-ar
	# make
注意：openssh不需要make install，编译好后，手动拷贝ELF文件至开发板即可。

###7. 拷贝至目标机(ARM开发板)
将/arm-ssh/source/openssh下的:

* scp  sftp  ssh  ssh-add  ssh-agent  ssh-keygen  ssh-keyscan 拷贝到目标板/usr/local/bin

* moduli ssh\_config sshd\_config拷贝到目标板 /usr/local/etc   

* sftp-server  ssh-keysign 拷贝到目标板 /usr/local/libexec   

* sshd 拷贝到目标板 /usr/local/sbin/ 


* 拷贝install/zlib-1.2.3/lib/libz.so.1.2.* 到 arm开发板的/lib/下，并建立软连接:
	
		ln -s libz.so.1.2.* libz.so.1
		ln -s libz.so.1.2.* libz.so

注意：动态连接库必须放在/lib/目录下。    
libz.so.1.2.*是一个动态连接库文件，但是程序调用的时候使用的是libz.so.1这个名称，所以要建立软链接；

###8. 生成Key文件

	# ssh-keygen -t rsa -f ssh_host_rsa_key -N ""
	# ssh-keygen -t dsa -f ssh_host_dsa_key -N ""
	# ssh-keygen -t ecdsa -f ssh_host_ecdsa_key -N ""
	# ssh-keygen -t ed25519 -f ssh_host_ed25519_key -N ""

将生成的 `ssh_host_*_key`这4个文件copy到目标板的 /usr/local/etc/目录下.
并配置权限为600;

###9. (ARM Linux下)添加sshd用户

修改目标板passwd文件：
在/etc/passwd 中添加下面这一行

	sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin

###10. (ARM Linux下)启动sshd

在目标板上运行：

	# /usr/local/sbin/sshd
	# ps | grep sshd

可以用ps命令查看sshd是否在工作
如果运行的过程中有提示缺少动态连接库，可以在主机上搜索相应文件，拷贝到目标板/lib/目录下面，注意创建软连接！

###11. (ARM Linux下)配置sshd_config

	# vi /usr/local/etc/sshd_config
	

* `PermitRootLogin yes`, 允许ROOT用户登录。    
* `RSAAuthentication yes`, RSA验证打开。   
* `PubkeyAuthentication yes`，公钥验证打开。   
* `AuthorizedKeysFile  /root/.ssh/authorized_keys`，认证公钥文件  
* `PasswordAuthentication yes`，密码认证打开。
* `PermitEmptyPasswords no`, 禁止空密码。

保存，重启sshd，就可以通过密码和RSA公钥两种方法通过ssh登录arm linux。



----


