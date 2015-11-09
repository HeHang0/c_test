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

#define PORT 13490
#define IP "127.0.0.1"
#define MAXDATESIZE 100
#define NAME 3
#define USER 5
#define ACCOUNTSIZE 2
#define TYPE 4

/****************************struct*********************************/	
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
	struct frdlist
	{
		char numlist[NAME];
	};

	struct myfrd
	{
		char type[TYPE];
		char numfrd[NAME];
		struct frdlist list[USER];
	};
/*******************************************************************/

	int sockfd,numbytes,len,bytes_send,i,sel;
	char buf0[MAXDATESIZE];	
	fd_set fdsr;
	struct timeval timev;
	pthread_t thread[2];

	struct login LOGIN;
	struct buffer BUF;
	struct massage MSG;
	struct myfrd friend;
	struct sockaddr_in their_addr;


/*************************loginaccount******************************/
void loginaccount()
{
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
	if(recv(sockfd,buf0,sizeof(buf0),0) == -1)
	{
		perror("recv");
		exit(1);
	}
	else
	{
	printf("%s\n",buf0);
	}
}
/*******************************************************************/

/**************************addfriend********************************/
void addfriend()
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
}
/*******************************************************************/

/**************************startchat********************************/
void startchat()
{
	while(1)
	{
		bzero(MSG.msg,MAXDATESIZE);
		printf("\nsendmsg:");
		scanf("%s",MSG.msg);
		MSG.msg[strlen(MSG.msg)] = '\0';
		if((bytes_send = send(sockfd,&MSG,sizeof(struct massage),0)) == -1)
		{
			perror("send");
			exit(1);
		}
		else if(bcmp(MSG.msg,"exit",4) == 0)
			break;
	}
}
/*******************************************************************/

/****************************sendchat*******************************/
void sendchat()
{
	printf("请输入好友账号以开始聊天：");
	bzero(MSG.msg,MAXDATESIZE);
	scanf("%s",MSG.msg);
	len = strlen(MSG.msg);
	MSG.msg[len] = '\0';
	if((send(sockfd,&MSG,sizeof(struct massage),0)) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
	printf("send%s:%s\n",MSG.nummsg,MSG.msg);
	startchat();
	}
}
/*******************************************************************/

/****************************thread*********************************/
void *recvthread()
{	
	while(1)
	{
		timev.tv_sec = 0;//设置超时
		timev.tv_usec = 0;
		sel = select(sockfd,&fdsr,NULL,NULL,&timev);//&timev);
		if(sel < 0)
		{
			perror("select");
			break;
		}
		FD_SET(sockfd,&fdsr);
			if(FD_ISSET(sockfd,&fdsr))
			{
				bzero(&BUF,sizeof(struct buffer));

				if((numbytes = recv(sockfd,&BUF,sizeof(struct buffer),0)) == -1)
				{
					perror("recv");
				}
				else if(numbytes <= sizeof(struct buffer))
				{	
					if(bcmp(&BUF,"00",2) == 0)
						printf("\n服务器：%s\n",BUF.buf);
					else if(bcmp(BUF.buf,"This chat is over!",18) == 0)
					{
						printf("服务器：This chat is over!\n");
						break;
					}
					else if(strlen(BUF.buf) != 0)
						printf("\n客户端%s：%s\n",BUF.numbuf,BUF.buf);
				}
				
			}
	}
}
void thread_create()
{
	bzero(&thread,sizeof(thread));
//	if(pthread_create(&thread[0], NULL, sendthread, NULL) != 0)
//		printf("线程1创建失败!\n");
//	else
//		printf("线程1创建成功!\n");
	if(pthread_create(&thread[1], NULL, recvthread, NULL) != 0)
		printf("线程2创建失败!\n");
	else
		printf("线程2创建成功!\n");
}
void thread_wait()
{
	if(thread[0] != 0)
	{
		pthread_join(thread[0],NULL);
		printf("线程1已经结束\n");
	}
	if(thread[1] != 0)
	{
		pthread_join(thread[1],NULL);
		printf("线程2已经结束\n");
	}
}
/*******************************************************************/

/*******************************************************************/
void loop()
{
while(1)
{
	bzero(MSG.msg,MAXDATESIZE);
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
			addfriend();
		}
		else if(bcmp(MSG.msg,"chat",4) == 0)
		{
			sendchat();
		}
		if((bcmp(MSG.msg,"Y",1) == 0) || (bcmp(MSG.msg,"y",1) == 0))
		{
			printf("开始聊天\n");
			startchat();
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
	if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		exit(1);
	}
	else
	{
		printf("connectd!\n");//在这里，加一个成功连接后的消息，来输入确实成功的消息
	}

	loginaccount();
	pthread_mutex_t mut;
	pthread_mutex_init(&mut,NULL);
	thread_create();
	loop();

	close(sockfd);
	
	return 0;
}
