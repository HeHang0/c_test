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
#define NAME 3

	int sockfd,numbytes,len,bytes_send;
	char buf0[MAXDATESIZE];
	struct sockaddr_in their_addr;
	struct login
	{
		char loginname[16];
		char passwd[16];
	};
	struct buffer
	{
		char numbuf[NAME];
		char buf[MAXDATESIZE];
	};
	struct massage
	{
		char nummsg[NAME];
		char msg[MAXDATESIZE];
	};
	
	struct login LOGIN;
	struct buffer BUF;
	struct massage MSG;
		
void loopsend()
{
while(1)
{
	bzero(MSG.msg,MAXDATESIZE);
//        char *str="this is a good msg";
//	memcpy(msg,str,strlen(str));	
	//这一步你忘记掉了，一定要给len赋值，把要发送的消息长度赋给len
	printf("客户端：");
	scanf("%s",MSG.msg);
	len = strlen(MSG.msg);
	printf("%s = %d\n",MSG.msg,len);
	MSG.msg[len] = '\0';


	if((bytes_send = send(sockfd,&MSG,sizeof(struct massage),0)) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
		printf("send success！\n");//这里发送成功了也输出一条信息便于调试
		if(bcmp(MSG.msg,"add",3) == 0)
		{	
			printf("请输入号码查找好友：");
			scanf("%s",MSG.msg);
			len = strlen(MSG.msg);
			MSG.msg[len] = '\0';			
			if((bytes_send = send(sockfd,&MSG,sizeof(struct massage),0)) == -1)
			{
				perror("send");
				exit(1);
			}
			bzero(buf0,MAXDATESIZE);
			if((numbytes = recv(sockfd,buf0,MAXDATESIZE,0)) == -1)
			{
				perror("recv");
//				exit(1);
			}
			else			
				printf("%s\n",buf0);
		}
	}
/*
	bzero(BUF,sizeof(struct buffer));
	if((numbytes = recv(sockfd,BUF,sizeof(struct buffer),0)) == -1)
	{
		perror("recv");
//		exit(1);
	}
	else if(bcmp(BUF.numbuf,"00",2) != 0)
	{
		printf("received %d bytes %s\n",numbytes,buf);
	}
*/
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


	bzero(&(LOGIN.loginname),sizeof(&(LOGIN.loginname)));

	printf("Name:");
	scanf("%s",LOGIN.loginname);
	strcpy(MSG.nummsg,LOGIN.loginname);
	len = strlen(MSG.nummsg);

	MSG.nummsg[len] = '\0';
	
	
	len = strlen(LOGIN.loginname);
	LOGIN.loginname[len] = '\0';
	printf("Passwd:");
	scanf("%s",LOGIN.passwd);
	len = strlen(LOGIN.passwd);
	LOGIN.passwd[len] = '\0';


	if(send(sockfd,&(LOGIN),sizeof(struct login),0) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
	printf("验证已发送!\n");
	}
//	sleep(2);
	if(recv(sockfd,buf0,sizeof(buf0),0) == -1)
	{
		perror("recv");
		exit(1);
	}
	else
	{
	printf("%s\n",buf0);
	}

	loopsend();

	close(sockfd);
	
	return 0;
}
