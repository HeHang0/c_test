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
#include <pthread.h>
#include "pb.pb-c.h"

#define PORT 13490
#define IP "127.0.0.1"
#define MAXDATESIZE 100
#define LOGINPWD 16
#define USER 10
#define ACCOUNTSIZE 2
#define TYPE 4


/*******************************************************************/

	int sockfd,numbytes,len,i,j,sel;
	int loginnumber = 0,friendnumber = 0,tempchatnumber = 0;
	int symbol = 0;
	char password[16];
	char writebuf[MAXDATESIZE];	
	uint8_t * recvbuf;
	struct timeval timev;
	pthread_t thread;

	Login LOGIN = LOGIN__INIT;
	Buffer *BUF = NULL;
	Buffer MSG = BUFFER__INIT;
	void *buf = NULL;
	struct sockaddr_in their_addr;

/*******************************************************************/
static int malloc_login_info(Login *LOGIN)
{
    LOGIN->passwd = (char*)malloc(LOGINPWD);
    if (!LOGIN->passwd)
    {
    goto FAILED;
    }
    return 0;
FAILED:
    fprintf(stdout, "malloc error.errno:%u,reason:%s\n",
        errno, strerror(errno));
    return -1;
}
static void set_login_info(Login *LOGIN,int loginnumber,char* password,int pwdlen)
{
	LOGIN->loginname = loginnumber;
	memcpy(LOGIN->passwd,password,pwdlen);
	*(LOGIN->passwd+pwdlen) = '\0';
}
static void free_login(Login *LOGIN)
{
    if (LOGIN->passwd)
    {
    free(LOGIN->passwd);
    LOGIN->passwd = NULL;
    }
}
static int malloc_msg_info(Buffer *MSG)
{
    MSG->buf = (char*)malloc(MAXDATESIZE);
    if (!MSG->buf)
    {
    goto FAILED;
    }
    return 0;
FAILED:
    fprintf(stdout, "malloc error.errno:%u,reason:%s\n",
        errno, strerror(errno));
    return -1;
}
static void set_msg_info(Buffer *MSG,int length,int cmd,int number,int friend,void *writebuf)
{
	MSG->length = length;
	MSG->cmd = cmd;
	MSG->number = number;
	MSG->friend_ = friend;
	memcpy(MSG->buf,writebuf,length);
	*(MSG->buf+length) = '\0';
}
static void free_msg(Buffer *MSG)
{
	if (MSG->buf)
	{
		free(MSG->buf);
		MSG->buf = NULL;
	}
}
/*******************************************************************/

/*************************loginaccount******************************/
void loginaccount()
{
while(1)
{
	if((symbol == 1) && (strcmp(BUF->buf,"Login success!",14) == 0))
	{
		symbol = 0;
			break;
	}
	else
	{
	printf("Name:");
	scanf("%d",&loginnumber);

	printf("Passwd:");
	scanf("%s",password);
	
	malloc_login_info(&LOGIN);
	set_login_info(&LOGIN,loginnumber,password,strlen(password));
	len = login__get_packed_size(&LOGIN);
    buf = malloc(len);
    login__pack(&LOGIN, buf);
    free_login(&LOGIN);
    malloc_msg_info(&MSG);
	set_msg_info(&MSG,len,1,loginnumber,0,buf);
	free(buf);
	
	len = buffer__get_packed_size(&MSG);
    buf = malloc(len);
    buffer__pack(&MSG, buf);
    free_msg(&MSG);
	
	if(send(sockfd,buf,len,0) == -1)
	{
		perror("send");
		exit(1);
	}
	else
		printf("验证已发送!\n");
	free(buf);
	sleep(1);
	}
}
	printf("登录成功！\n");
}
/*******************************************************************/

/**************************addfriend********************************/
void addfriend()
{
	printf("请输入号码查找好友：");
	scanf("%d",&friendnumber);
	bzero(writebuf,MAXDATESIZE);
	memcpy(writebuf,"addfriend",9);
	len = strlen(writebuf);
	writebuf[len] = '\0';
	malloc_msg_info(&MSG);
	set_msg_info(&MSG,len,2,loginnumber,friendnumber,writebuf);
	len = buffer__get_packed_size(&MSG);
    buf = malloc(len);
    buffer__pack(&MSG, buf);
    free_msg(&MSG);
	if(send(sockfd,buf,len,0) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	printf("添加好友\n");
	free(buf);
	symbol = 0;
}
/*******************************************************************/

/****************************sendchat*******************************/
void sendchat()
{
	printf("请输入好友账号以开始聊天：");
	scanf("%d",&friendnumber);
	bzero(writebuf,MAXDATESIZE);
	memcpy(writebuf,"Chat! reply:Y/N",15);
	writebuf[15] = '\0';
	malloc_msg_info(&MSG);
	set_msg_info(&MSG,15,3,loginnumber,friendnumber,writebuf);
	len = buffer__get_packed_size(&MSG);
    buf = malloc(len);
    buffer__pack(&MSG, buf);
    free_msg(&MSG);
	if((send(sockfd,buf,len,0)) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
		free(buf);
		symbol = 4;
	}
}
/*******************************************************************/

/**************************startchat********************************/
void startchat()
{
if(strlen(writebuf) > 0)
{
	malloc_msg_info(&MSG);
	set_msg_info(&MSG,len,3,loginnumber,tempchatnumber,writebuf);
	len = buffer__get_packed_size(&MSG);
    buf = malloc(len);
    buffer__pack(&MSG, buf);
    free_msg(&MSG);
	if(send(sockfd,buf,len,0) == -1)
	{
		perror("send");
		exit(1);
	}
	else if(strcmp(writebuf,"exit",4) == 0)	
		symbol = 0;
	free(buf);
}
}
/*******************************************************************/

/***************************groupchat*******************************/
void groupchat()
{
if(strlen(writebuf) > 0)
{
	malloc_msg_info(&MSG);
	set_msg_info(&MSG,len,4,loginnumber,0,writebuf);
	len = buffer__get_packed_size(&MSG);
   	buf = malloc(len);
   	buffer__pack(&MSG, buf);
   	free_msg(&MSG);
	if(send(sockfd,buf,len,0) == -1)
	{
		perror("send");
		exit(1);
	}
	else if(strcmp(writebuf,"exit",4) == 0)
		symbol = 0;
	free(buf);
}
}
/*******************************************************************/

/****************************thread*********************************/
void *looprecv()
{	
	while(1)
	{
		recvbuf = malloc(MAXDATESIZE*2);
		if((numbytes = recv(sockfd,recvbuf,MAXDATESIZE*2,0)) == -1)
		{
			perror("recv");
			exit(1);
		}
		else
		{
			BUF = buffer__unpack(NULL, numbytes, recvbuf);
			switch(BUF->cmd)
			{
				case 0: printf("\n服务器：%s\n",BUF->buf);break;
				case 1:	
					{
						printf("\n服务器：%s\n",BUF->buf);
						symbol = 1;
						break;
					}
				case 2:	printf("\n服务器：%s\n",BUF->buf);break;
				case 3:	
					{
						printf("\n客户端%d：%s\n",BUF->number,BUF->buf);
						tempchatnumber = BUF->number;
						break;
					}
				case 4:printf("\n客户端%d：%s\n",BUF->number,BUF->buf);break;
			}
		}
		free(recvbuf);
	}
	pthread_exit(NULL);
}
void thread_create()
{
	bzero(&thread,sizeof(thread));
	if(pthread_create(&thread, NULL, looprecv, NULL) != 0)
		printf("线程looprecv创建失败!\n");
}
void thread_wait()
{
	if(thread != 0)
	{
		pthread_join(thread,NULL);
		printf("线程looprecv已经结束\n");
	}
}

/*******************************************************************/

/*******************************************************************/
void loop()
{
while(1)
{
	bzero(writebuf,MAXDATESIZE);
	printf("客户端：");
	scanf("%s",writebuf);
	len = strlen(writebuf);
	writebuf[len] = '\0';

	if(strcmp(writebuf,"/add",4) == 0)
	{
		symbol = 2;
		bzero(writebuf,MAXDATESIZE);
	}
	else if(strcmp(writebuf,"/chat",5) == 0)
	{
		symbol = 3;
		bzero(writebuf,MAXDATESIZE);
	}
	else if((strcmp(writebuf,"Y") == 0) || (strcmp(writebuf,"y") == 0))
	{
		symbol = 4;
		printf("Start chat:");
		bzero(writebuf,MAXDATESIZE);
	}
	else if(strcmp(writebuf,"/groupchat",10) == 0)
	{
		symbol = 5;
		printf("Start groupchat:");
		bzero(writebuf,MAXDATESIZE);
	}
	switch(symbol)
	{
		case 2:
		{
			addfriend();
			break;
		}
		case 3:
		{
			sendchat();
			break;
		}
		case 4:
		{
			startchat();
			break;
		}
		case 5:
		{
			groupchat();
			break;
		}
	}
}
}
/*******************************************************************/

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
	bzero(buf0,MAXDATESIZE);

	if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		exit(1);
	}
	else
	{
		printf("connectd!\n");//在这里，加一个成功连接后的消息，来输入确实成功的消息
	}

	pthread_mutex_t mut;
	pthread_mutex_init(&mut,NULL);
	thread_create();
	loginaccount();
	loop();


	close(sockfd);
	
	return 0;
}
