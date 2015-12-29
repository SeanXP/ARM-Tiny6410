/****************************************************************
            Copyright (C) 2014 All rights reserved.
					      									  
    > File Name:         < echo_server.c >
    > Author:            < Sean Guo >
    > Mail:              < iseanxp+code@gmail.com >
    > Created Time:      < 2014/06/19 >
    > Last Changed: 	 < 2015/11/30 >
    > Description:		echo server for ARM

	//{{{
 	int bind(int sockfd, struct sockaddr * my_addr, int addrlen);
 	bind()用来设置给参数sockfd 的socket 一个名称. 
 	此名称由参数my_addr 指向一个sockaddr 结构, 对于不同的socket domain 定义了一个通用的数据结构

 	struct sockaddr
 	{
    	unsigned short int sa_family;
	   	char sa_data[14];
	};
	
	1、sa_family 为调用socket()时的domain 参数, 即AF_xxxx 值.
	2、sa_data 最多使用14 个字符长度.

	此sockaddr 结构会因使用不同的socket domain 而有不同结构定义, 
	例如使用AF_INET domain,其socketaddr 结构定义便为
	struct socketaddr_in
	{
		unsigned short int sin_family;
	    uint16_t sin_port;
		struct in_addr sin_addr;
		unsigned char sin_zero[8];
	};

	struct in_addr
	{
		uint32_t s_addr;
	};
	
	1、sin_family 即为sa_family
    2、sin_port 为使用的port 编号
    3、sin_addr. s_addr 为IP 地址 sin_zero 未使用.
    参数 addrlen 为sockaddr 的结构长度.

    返回值：成功则返回0, 失败返回-1, 错误原因存于errno 中.

	错误代码：
	1、EBADF 参数sockfd 非合法socket 处理代码.
	2、EACCESS 权限不足
	3、ENOTSOCK 参数sockfd 为一文件描述词, 非socket.
	//}}}

	Usage: ./echo_server

****************************************************************/
//{{{ include files
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>	// waitpid()
#include <stdlib.h>		// exit();
#include <string.h>		// bzero();
#include <netinet/in.h>	// struct sockaddr_in;
#include <time.h>		// time();
#include <arpa/inet.h>  // inet_pton();
#include <unistd.h>     // write();
#include <errno.h>		// errno
#include <signal.h>		// SIGCHLD
//}}}

#define MAXLINE 	4096	/* max text line length */
#define LISTENQ     1024    /* 2nd argument to listen() , 排队的最大连接数*/
#define LISTEN_PORT	9669	//服务器监听端口

//参数: 已连接的socket描述符.
//功能: echo此socket发送的一切数据;
//阻塞函数, 直到对方socket关闭.
void str_echo(int sockfd);

//信号处理函数, 将等待一个子进程的结束。
void sig_child(int signo);

int main(int argc, char **argv)
//{{{
{
	int	listenfd, connfd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len;
	char buffer[MAXLINE];
	pid_t child_pid;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(LISTEN_PORT);

	// int bind(int sockfd, struct sockaddr * my_addr, int addrlen);
	// bind()用来设置给参数sockfd 的socket 一个名称. 
	// 此名称由参数my_addr 指向一个sockaddr 结构, 对于不同的socket domain 定义了一个通用的数据结构
	bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	// int listen(int s, int backlog);
	// listen()用来监听描述符s 的socket连接请求. 
	// 参数backlog 指定同时能处理的最大连接要求, 如果连接数目达此上限则client 端将收到ECONNREFUSED 的错误. 
	// listen()并未开始接收连接请求, 只设置socket 为listen 模式,真正接收client 端连线的是accept().
	// 通常listen()会在socket(), bind()之后调用, 接着才调用accept().
	// 成功则返回0, 失败返回-1, 错误原因存于errno
	// listen()只适用SOCK_STREAM 或SOCK_SEQPACKET 的socket 类型. 
	// 如果socket 为AF_INET 则参数backlog 最大值可设至128.
	listen(listenfd, LISTENQ);

	signal(SIGCHLD, sig_child);	//为SIGCHLD匹配自定义的函数, 使得处理子进程僵死的问题.

	//主进程就为一个监听端口, 为每个连接fork一个新的进程.
	for ( ; ; ) {
		addr_len = sizeof(client_addr);
		// int accept(int s, struct sockaddr * addr, int * addrlen);
		// accept()用来接受描述符s 的socket连接请求. 
		// socket 必需先经bind()、listen()函数处理过, 
		// 当有连接请求进来时, accept()会返回一个新的socket 处理代码,  往后的数据传送与读取就是经由新的socket处理, 
		// 而原来参数s 的socket 能继续使用accept()来接受新的连线要求. 
		// 连线成功时, 参数addr 所指的结构会被系统填入远程主机的地址数据, 参数addrlen 为scokaddr 的结构长度.
		// 成功则返回新的socket 处理代码, 失败返回-1, 错误原因存于errno 中.
		connfd = accept(listenfd, (struct sockaddr *) &client_addr, &addr_len);

		//创建子进程处理客户端请求, 主进程继续监听.
		child_pid = fork();
		if(child_pid < 0)	//failed to fork a process.
		{
			fprintf(stderr, "error: failed in fork()\n");
			exit(1);
		}
		else if(child_pid == 0) //the child process.
		{
			close(listenfd);	//close listenfd in child process.
			str_echo(connfd);	//the task of child process - As a echo server.
			exit(0);
		}
		else	// the parent process.
			close(connfd);		//close connfd in parent process.
		//调用close()只会减少对应socket描述符的引用数, 当引用数为0才会清楚对应的socket.
	}
}//}}}

void str_echo(int sockfd)
//{{{
{
	ssize_t n;
	char buf[1024];

again:
	while( (n = read(sockfd, buf, 1024)) > 0)	//不断从sockfd中读取数据
		write(sockfd, buf, n);
	
	if(n < 0 && errno == EINTR)		//由于信号中断(EINTR)而没有读取到数据时, 返回while循环.
		goto again;
	else if( n < 0)	//无法读取数据
		perror("str_echo: read error");
}//}}}

//信号处理函数, 将等待一个子进程的结束。
void sig_child(int signo)
//{{{
{
	pid_t pid;
	int state;

	//pid = wait(&state);	//等待一个子进程的结束
	while( (pid = waitpid(-1, &state, WNOHANG)) > 0)	//使用非阻塞的waitpid等待可结束的所有子进程
		printf("child pid[%d] terminated.\n", pid);
}//}}}
