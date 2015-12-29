Ubuntu - NFS安装与配置

###安装NFS

	$ sudo apt-get install nfs-kernel-server
	$ sudo apt-get install portmap nfs-common

###修改配置文件

	$ sudo vim /etc/exports

添加:
	
	/var/nfsroot  *(rw,sync,no_root_squash,insecure)

解释:   

* /var/nfsroot：要共享的目录
* \*：允许所有的网段访问
* rw ：读写权限
* sync：资料同步写入内在和硬盘
* no\_root\_squash：nfs客户端共享目录使用者权限
* insecure: nfs安全上默认使用1024以下端口进行连接。因此很多情况下分配的端口不符合规定, 则无法挂载nfs目录。这里设置为(insecure)即可。

###创建挂载点(这里对应上面的/var/nfsroot)

	$ cd /var
	$ sudo mkdir nfsroot
	$ sudo chmod 777 nfsroot

###重启nfs服务(每次修改配置文件后都要重启才能生效)

	$ sudo /etc/init.d/nfs-kernel-server restart
	$ sudo /etc/init.d/portmap restart
	($ sudo /etc/init.d/rpcbind restart)

(新版本的Ubuntu, portmap 改为rpcbind)

注：nfs是一个RPC程序，使用它前，需要映射好端口，通过portmap设定。所以这里也要重启protmap服务

###测试

	$ showmount -e

改命令显示出共享出的目录
理论上会显示:

	Export list for ubuntu:
	/var/nfsroot *


现在可以在本机上尝试挂载一下：

	$ sudo mount -t nfs localhost:/var/nfsroot /mnt

注：localhost为本机linux的IP地址, 也可为127.0.0.1
这样就把共享目录挂到了/mnt目录，取消挂载用：

	$ sudo umount /mnt

如果用在嵌入式设备上挂载，要加上参数-o nolock
我在开发板上使用的挂载命令：
	
	$ mount -t nfs -o nolock 10.42.1.100:/var/nfsroot/ /mnt
这里假设10.42.1.100为nfs服务器的IP地址.