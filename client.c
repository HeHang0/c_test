#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 13490
#define IP "127.0.0.1"
#define MAXDATESIZE 100

	int sockfd,numbytes,len,bytes_send;
	char buf[MAXDATESIZE],msg[MAXDATESIZE];
	struct sockaddr_in their_addr;
void loopsend()
{
while(1)
{
	bzero(msg,sizeof(msg));
//        char *str="this is a good msg";
//	memcpy(msg,str,strlen(str));	
	//这一步你忘记掉了，一定要给len赋值，把要发送的消息长度赋给len
	printf("客户端：");
	scanf("%s",msg);
	len = strlen(msg);
	
	if((bytes_send = send(sockfd,msg,len,0)) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
		printf("send success！\n");//这里发送成功了也输出一条信息便于调试
	}
/*	bzero(buf,sizeof(buf));
	if((numbytes = recv(sockfd,buf,MAXDATESIZE,0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	else
	{
		printf("received %d bytes %s\n",numbytes,buf);
	}*/
}
//	sleep(2); //如果你send完以后马上关闭socket,那边会接收不到的，所以sleep2秒，这里一般是为了调试才加sleep的，真正应用的时候不能用sleep的，因为不会马上关闭socket.

}

int main()
{
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr.s_addr = inet_addr(IP);
	bzero(&(their_addr.sin_zero),8);
	//整体上逻辑是，客户端先连接，然后发一个字符串过去，等一下，再关闭。
	if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		exit(1);
	}
	else
	{
		printf("connectd!\n");//在这里，加一个成功连接后的消息，来输入确实成功的消息
	}
	
	loopsend();

	close(sockfd);
	
	return 0;
}
