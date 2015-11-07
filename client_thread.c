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
//	sleep(1);
	bzero(buf0,MAXDATESIZE);
	if((numbytes = recv(sockfd,buf0,MAXDATESIZE,0)) == -1)
	{
		perror("recv");
		exit(1);
	}
	else
		printf("服务器：%s\n",buf0);

}
/*******************************************************************/


/****************************thread*********************************/
void *sendthread()
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
		printf("send success！\n");
		if(bcmp(MSG.msg,"add",3) == 0)
		{
			addfriend();
		}
	}

}
	pthread_exit(NULL);

}
/*******************************************************************/
void *recvthread()
{
		
	while(1)
	{
		timev.tv_sec = 0;//设置超时
		timev.tv_usec = 0;
		FD_ZERO(&fdsr);//将set清零使集合中不含任何fd
		FD_SET(sockfd,&fdsr);//将fd加入set集合
	
		sel = select(sockfd,&fdsr,NULL,NULL,&timev);//&timev);
		if(sel < 0)
		{
			perror("select");
			break;
		}
			if(FD_ISSET(sockfd,&fdsr))
			{
				bzero(&BUF,sizeof(struct buffer));

				if((numbytes = recv(sockfd,&BUF,sizeof(struct buffer),0)) == -1)
				{
					perror("recv");
				}
				else if(numbytes <= sizeof(struct buffer))
				{
					if(bcmp(BUF.numbuf,"00",2) == 0)
						printf("服务器：%s\n",BUF.buf);
					else
						printf("客户端%s：%s\n",BUF.numbuf,BUF.buf);
				}
				
			}
				
	}
	pthread_exit(NULL);
}
/*******************************************************************/
void thread_create()
{
	bzero(&thread,sizeof(thread));
	if(pthread_create(&thread[0], NULL, sendthread, NULL) != 0)
		printf("线程1创建失败!\n");
//	else
//		printf("线程1创建成功!\n");
	if(pthread_create(&thread[1], NULL, recvthread, NULL) != 0)
		printf("线程2创建失败!\n");
//	else
//		printf("线程2创建成功!\n");
}
/*******************************************************************/
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

	loginaccount();
	pthread_mutex_t mut;
	pthread_mutex_init(&mut,NULL);
	thread_create();
	thread_wait();

	close(sockfd);
	
	return 0;
}
