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
#define USER 10
#define ACCOUNTSIZE 2
#define TYPE 4

/****************************struct*********************************/	
	struct login
	{
		int loginname;
		char passwd[16];
	};
	struct header
	{
		int length;
		int cmd;//1登录，2加好友，3聊天，4群聊，5好友列表
		int number;
		int friend;
	};
	struct buffer
	{
		struct header head;
		char buf[MAXDATESIZE];
	};
	struct massage
	{
		struct header head;
		char msg[MAXDATESIZE];
	};
	struct frdlist
	{
		int numlist;
	};

/*******************************************************************/

	int sockfd,numbytes,len,bytes_send,i,j,sel,tempchatnumber;
	char buf0[MAXDATESIZE];	
	fd_set fdsr;
	struct timeval timev;
	pthread_t recvthread;

	struct login LOGIN;
	struct buffer BUF;
	struct massage MSG;
	struct frdlist friend[USER];
	struct sockaddr_in their_addr;


/*************************loginaccount******************************/
void loginaccount()
{
while(1)
{
	printf("Name:");
	scanf("%d",&LOGIN.loginname);
	MSG.head.number = LOGIN.loginname;

	printf("Passwd:");
	scanf("%s",LOGIN.passwd);
	MSG.head.cmd = 1;
	memcpy(MSG.msg,&LOGIN,sizeof(struct login));
	if(send(sockfd,&MSG,sizeof(struct massage),0) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
	printf("验证已发送!\n");
	}
	while(1)
	{
		if((strcmp(BUF.buf,"Login success!") == 0) || (strcmp(BUF.buf,"Infor error") == 0))
		{
			strcpy(buf0,BUF.buf);
			break;
		}
	}
	if(strcmp(buf0,"Login success!") == 0)
		break;
}
	printf("登录成功！\n");
}
/*******************************************************************/

/**************************addfriend********************************/
void addfriend()
{	
	printf("请输入号码查找好友：");
	bzero(MSG.msg,MAXDATESIZE);
	scanf("%s",MSG.msg);
	len = strlen(MSG.msg);
	MSG.msg[len] = '\0';			
	if(send(sockfd,&MSG,sizeof(struct massage),0) == -1)
	{
		perror("send");
		exit(1);
	}
}
/*******************************************************************/

/**************************startchat********************************/
void startrecvchat()
{
	while(1)
	{
		bzero(MSG.msg,MAXDATESIZE);
		MSG.head.friend = tempchatnumber;
		printf("\nsendmsg%d:",MSG.head.friend);
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
void startsendchat()
{
	while(1)
	{
		bzero(MSG.msg,MAXDATESIZE);
		printf("\nsendmsg%d:",MSG.head.friend);
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

/***************************groupchat*******************************/
void groupchat()
{
	while(1)
	{
		bzero(MSG.msg,MAXDATESIZE);
		printf("\ngroupchat:");
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

/***************************friendlist******************************/
void friendlist()
{
		bzero(MSG.msg,MAXDATESIZE);
		memcpy(MSG.msg,"friendlist",10);
		MSG.msg[10] = '\0';
		if(send(sockfd,&MSG,sizeof(struct massage),0) == -1)
		{
			perror("send");
			exit(1);
		}
		else
			printf("请求好友列表成功！\n");
}
/*******************************************************************/

/****************************sendchat*******************************/
void sendchat()
{
	printf("请输入好友账号以开始聊天：");
	bzero(MSG.msg,MAXDATESIZE);
	scanf("%d",&MSG.head.friend);
	memcpy(MSG.msg,"Chat! reply:Y/N",15);
	MSG.msg[15] = '\0';
	if((send(sockfd,&MSG,sizeof(struct massage),0)) == -1)
	{
		perror("send");
		exit(1);
	}
	else
	{
//	printf("send%d:%s\n",MSG.head.friend,MSG.msg);
	startsendchat();
	}
}
/*******************************************************************/


/****************************thread*********************************/
void *looprecv()
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
			else if(numbytes <= sizeof(struct buffer) && (strlen(BUF.buf) > 0))
			{
				switch(BUF.head.cmd)
				{
					case 0: printf("\n服务器：%s\n",BUF.buf);break;
					case 1:	printf("\n服务器：%s\n",BUF.buf);break;
					case 2:	printf("\n服务器：%s\n",BUF.buf);break;
					case 3:	
						{
							printf("\n客户端%d：%s\n",BUF.head.number,BUF.buf);
							tempchatnumber = BUF.head.number;
							break;
						}
					case 4:printf("\n客户端%d：%s\n",BUF.head.number,BUF.buf);break;
					case 5:
						{
							memcpy(friend,BUF.buf,sizeof(struct frdlist)*USER);
							for(i = 0,j = 0;i < USER;i++)
							{
								if(friend[i].numlist != 0)
									printf("friend%d：%d\n",++j,friend[i].numlist);
							}
						}
				}
			}
		}
	}
	pthread_exit(NULL);
}
void thread_create()
{
	bzero(&recvthread,sizeof(recvthread));
	if(pthread_create(&recvthread, NULL, looprecv, NULL) != 0)
		printf("线程recvthread创建失败!\n");
//	else
//		printf("线程recvthread创建成功!\n");
}
void thread_wait()
{
	if(recvthread != 0)
	{
		pthread_join(recvthread,NULL);
		printf("线程recvthread已经结束\n");
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
	MSG.msg[len] = '\0';

		printf("send success！\n");//这里发送成功了也输出一条信息便于调试
		if(bcmp(MSG.msg,"add",3) == 0)
		{
			MSG.head.cmd = 2;
			addfriend();
		}
		else if(bcmp(MSG.msg,"chat",4) == 0)
		{
			MSG.head.cmd = 3;
			sendchat();
		}
		else if((bcmp(MSG.msg,"Y",1) == 0) || (bcmp(MSG.msg,"y",1) == 0))
		{
			MSG.head.cmd = 3;
			printf("开始聊天了\n");
			startrecvchat();
		}
		else if(bcmp(MSG.msg,"groupchat",9) == 0)
		{
			MSG.head.cmd = 4;
			groupchat();
		}
		else if(bcmp(MSG.msg,"friendlist",10) == 0)
		{
			MSG.head.cmd = 5;
			friendlist();
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
