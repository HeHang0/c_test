#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "pb.pb-c.h"

#define PORT 13490
#define BACKLOG 10
#define IP "127.0.0.1"
#define MAXDATESIZE 100
#define LOGINNAME 16
#define LOGINPWD 16
#define USER 10
#define USERNAME 7
#define USERSEX 4
#define USERAGE 3
	
/******************************struct*******************************/
	struct frdlist
	{
		int numlist;
	};

	struct myfrd
	{
		int numfrd;
		struct frdlist list[USER];
	};
	struct selectclient
	{
		int clientacc;
		int clientfd;
	};
/*******************************************************************/
	
/******************************declar*******************************/
	int sockfd,newfd,len,i,j,k,l,m,n;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int YES = 1,symbol = 0;

	fd_set fdsr;
	pthread_t recvthread;
	int maxsock,conn_amount,sel;
	int client[BACKLOG],numbytes;
	char buf0[MAXDATESIZE];
	int loginnumber;
	char password[16];
	
	struct timeval timev;
	Login *LOGIN = NULL;
	Login LOGIN0[USER] = {LOGIN__INIT};
	Buffer *BUF = NULL;
	Buffer MSG = BUFFER__INIT;
	void* buf = NULL;
	struct myfrd friend[USER];
	struct selectclient cltfd[USER];
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
static void free_login_info(Login *LOGIN)
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
static void set_msg_info(Buffer *MSG,int length,int cmd,int number,int friend,void *buf0)
{
	MSG->length = length;
	MSG->cmd = cmd;
	MSG->number = number;
	MSG->friend_ = friend;
	memcpy(MSG->buf,buf0,length);
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
/****************************USERINFO*******************************/
void userinfo()
{
	strcpy(password,"123456");

	for(j = 0;j < USER;j++)
	{
		friend[j].numfrd = 11+j;
		cltfd[j].clientacc = 11+j;
		if(malloc_login_info(&LOGIN0[j]) == -1)
		{
			printf("申请内存不成功\n");
			exit(0);
		}
		set_login_info(&LOGIN0[j],11+j,password,6);
	for(k = 0;k < USER;k++)
		friend[j].list[k].numlist = 0;
	}
	

}
/*******************************************************************/

/*****************************LOGIN*********************************/
void clientlogin()
{
	if(LOGIN != NULL)
	{
		free(LOGIN);
		LOGIN = NULL;
	}
	buf = malloc(BUF->length);
	strcpy(buf,BUF->buf);
	LOGIN = login__unpack(NULL, BUF->length, buf);
	free(buf);
	for(k = 0;k < USER;k++)
	{
		if((LOGIN->loginname == LOGIN0[k].loginname) && (bcmp(LOGIN->passwd,LOGIN0[k].passwd,6) == 0))
		{
			bzero(&buf0,MAXDATESIZE);
			memcpy(buf0,"Login success!",14);
			buf0[14] = '\0';
			malloc_msg_info(&MSG);
			
			set_msg_info(&MSG,14,1,0,0,buf0);
			len = buffer__get_packed_size(&MSG);
			
	    	buf = malloc(len);
	    	buffer__pack(&MSG, buf);
	    	free_msg(&MSG);

			if(send(newfd,buf,len,0) == -1)
			{
				perror("send");
				exit(1);
			}
			else
			{
				for(l = 0;l < USER;l++)
				{
					if(LOGIN->loginname == cltfd[l].clientacc && cltfd[l].clientfd == 0)
					{
						cltfd[l].clientfd = newfd;
						printf("客户端%d已登录%d！\n",cltfd[l].clientacc,cltfd[l].clientfd);
						break;
					}
				}
			}
			free(buf);
			break;
		}
	}
	if(l == USER)
	{
		bzero(buf0,MAXDATESIZE);
		memcpy(buf0,"Already online",14);
		buf0[14] = '\0';
		malloc_msg_info(&MSG);
		set_msg_info(&MSG,14,1,0,0,buf0);
		len = buffer__get_packed_size(&MSG);
    	buf = malloc(len);
    	buffer__pack(&MSG, buf);
    	free_msg(&MSG);
		if(send(newfd,buf,len,0) == -1)
		{
			perror("send");
		}
		free(buf);
		printf("账号已在线！\n");
	}
	if(k == USER)
	{
		bzero(buf0,MAXDATESIZE);
		memcpy(buf0,"Infor error",12);
		buf0[12] = '\0';
		malloc_msg_info(&MSG);
		set_msg_info(&MSG,12,1,0,0,buf0);
		len = buffer__get_packed_size(&MSG);
    	buf = malloc(len);
    	buffer__pack(&MSG, buf);
    	free_msg(&MSG);
		if(send(newfd,buf,len,0) == -1)
		{
			perror("send");
		}
		free(buf);
		printf("客户端 验证失败%d\n",newfd);
	}
}
/*******************************************************************/	

/************************addfriend**********************************/
void addfriend()
{	
		for(m = 0;m < USER;m++)
		{
			if(cltfd[m].clientacc == BUF->number)
				break;
		}
		for(n = 0;n < USER;n++)
		{
			if(cltfd[n].clientacc == BUF->friend_)
				break;
		}
		for(j = 0;j < USER;j++)
		{
			if(BUF->friend_ == LOGIN0[j].loginname)
			{
			for(k = 0;k < USER;k++)
			{
				if(friend[k].numfrd == BUF->number)
				{
				for(l = 0;l < USER;l++)
				{
					if(friend[k].list[l].numlist == BUF->friend_)
					{
						bzero(buf0,MAXDATESIZE);
						memcpy(buf0,"already in list!",16);
						buf0[16] = '\0';
						malloc_msg_info(&MSG);
						set_msg_info(&MSG,16,2,BUF->number,BUF->friend_,buf0);
						len = buffer__get_packed_size(&MSG);
    					buf = malloc(len);
    					buffer__pack(&MSG, buf);
    					free_msg(&MSG);
						if(send(cltfd[m].clientfd,buf,len,0) == -1)
						{
							perror("send");
						}
						else
							break;
						free(buf);
					}
				}
				if(l == USER)
				{
					for(l = 0;l < USER;l++)
					if(friend[k].list[l].numlist == 0)
					{
						friend[k].list[l].numlist = BUF->friend_;
						bzero(buf0,MAXDATESIZE);
						memcpy(buf0,"add success!",12);
						buf0[12] = '\0';
						malloc_msg_info(&MSG);
						set_msg_info(&MSG,12,2,BUF->number,BUF->friend_,buf0);
						len = buffer__get_packed_size(&MSG);
    					buf = malloc(len);
    					buffer__pack(&MSG, buf);
    					free_msg(&MSG);
						if(send(cltfd[m].clientfd,buf,len,0) == -1)
						{
							perror("send");
						}
						free(buf);
						for(m = 0;m < USER;m++)
						{
							if(cltfd[m].clientacc == BUF->friend_)
							{
								bzero(buf0,MAXDATESIZE);
								memcpy(buf0,"already add by friend!",22);
								buf0[22] = '\0';
								malloc_msg_info(&MSG);
								set_msg_info(&MSG,22,2,BUF->friend_,BUF->number,buf0);
								len = buffer__get_packed_size(&MSG);
    							buf = malloc(len);
    							buffer__pack(&MSG, buf);
    							free_msg(&MSG);
								if(send(cltfd[n].clientfd,buf,len,0) == -1)
								{
									perror("send");
								}
								else
								{
									free(buf);
									break;
								}
							}
						}
						break;
					}
				}
				break;
				}
			}
			break;
			}
		}
		if(j == USER)
		{				
			bzero(buf0,MAXDATESIZE);
			memcpy(buf0,"not found!",10);
			buf0[10] = '\0';
			malloc_msg_info(&MSG);
			set_msg_info(&MSG,10,2,BUF->number,BUF->friend_,buf0);
			len = buffer__get_packed_size(&MSG);
    		buf = malloc(len);
    		buffer__pack(&MSG, buf);
    		free_msg(&MSG);
			if(send(cltfd[m].clientfd,buf,len,0) == -1)
			{
				perror("send");
			}
			free(buf);
		}
}

/*******************************************************************/

/****************************chat***********************************/
void chat()
{
	for(m = 0;m < USER;m++)
	{
		if(cltfd[m].clientacc == BUF->number)
			break;
	}
	for(n = 0;n < USER;n++)
	{
		if(cltfd[n].clientacc == BUF->friend_)
			break;
	}
	for(k = 0;k < USER;k++)
	{
		if(BUF->number == friend[k].numfrd)
			break;
	}	
	for(l = 0;l < USER;l++)
		if(friend[k].list[l].numlist == BUF->friend_)
			break;
	if(l < USER)			
	{
		for(n = 0;n < USER;n++)
		{
			if(cltfd[n].clientacc == BUF->friend_)
				break;
		}
		if((n < USER) && (cltfd[n].clientfd > 2))
		{
			len = buffer__get_packed_size(BUF);
			buf = malloc(len);
			buffer__pack(BUF,buf);
			if(send(cltfd[n].clientfd,buf,len,0) == -1)
			{
				perror("send");
			}
			free(buf);
		}
		else if(n == USER)
		{
			bzero(&buf0,MAXDATESIZE);
			memcpy(buf0,"Friend not online",17);
			buf0[17] = '\0';
			malloc_msg_info(&MSG);
			set_msg_info(&MSG,17,3,0,BUF->friend_,buf0);
			len = buffer__get_packed_size(&MSG);
			buf = malloc(len);
			buffer__pack(&MSG, buf);
			free_msg(&MSG);
			if((send(cltfd[m].clientfd,buf,len,0)) == -1)
			{
				perror("send");
				exit(1);
			}
			free(buf);
		}
	}
	else
	{
		bzero(&buf0,MAXDATESIZE);
		memcpy(buf0,"No friend",9);
		buf0[9] = '\0';
		malloc_msg_info(&MSG);
		set_msg_info(&MSG,9,3,0,BUF->friend_,buf0);
		len = buffer__get_packed_size(&MSG);
		buf = malloc(len);
		buffer__pack(&MSG, buf);
		free_msg(&MSG);
		if((send(cltfd[m].clientfd,buf,len,0)) == -1)
		{
			perror("send");
			exit(1);
		}
		free(buf);
	}
}

/*******************************************************************/

/***************************groupchat*******************************/
void groupchat()
{
	for(m = 0;m < USER;m++)
	{
		if(cltfd[m].clientacc == BUF->number)
			break;
	}
	for(k = 0;k <= conn_amount;k++)
	{
		if(client[k] > 0 && client[k] != cltfd[m].clientfd)
		{
			len = buffer__get_packed_size(BUF);
			buf = malloc(len);
			buffer__pack(BUF,buf);
			if(send(client[k],buf,len,0) == -1)
			{
				perror("send");
			}
			else
			{
				free(buf);
			}
		}
	}
}
/*******************************************************************/

/***************************looprecv********************************/
void *looprecv()
{
	conn_amount = 0;
	sin_size = sizeof(client_addr);
	maxsock = sockfd;
	while(1)
	{
		timev.tv_sec = 0;//设置超时
		timev.tv_usec = 0;
		FD_ZERO(&fdsr);//将set清零使集合中不含任何fd
		FD_SET(sockfd,&fdsr);//将fd加入set集合
		
		for(i = 0;i <= conn_amount;i++)
		{
			if(client[i] != 0)
			{
				FD_SET(client[i],&fdsr);
			}
		}
		
		sel = select(maxsock+1,&fdsr,NULL,NULL,&timev);//NULL);
		if(sel < 0)
		{
			perror("select");
			break;
		}
		for(i = 0;i <= conn_amount;i++)//检测所有套接字
		{			
			if(FD_ISSET(client[i],&fdsr) && (client[i] > 0))
			{
				buf = malloc(MAXDATESIZE*2);
				if((numbytes = recv(client[i],buf,MAXDATESIZE*2,0)) == -1)
				{
					perror("recv");
				}
				else if(numbytes == 0)
				{
					printf("客户端(%d) 关闭\n",i);
					close(client[i]);
					FD_CLR(client[i],&fdsr);
					for(j = 0;j < USER;j++)
						if(client[i] == cltfd[j].clientfd)
							cltfd[j].clientfd = 0;
					for(;i<conn_amount;i++)
					{
						client[i] = client[i+1];
					}
//					client[i] = 0;
					conn_amount--;
				}
				else
				{

					BUF = buffer__unpack(NULL, numbytes, buf);
					symbol = 1;
				}
				free(buf);
			}
		}
		if(FD_ISSET(sockfd,&fdsr))//检测是否有新客户端进入。
		{
			if((newfd = accept(sockfd,(struct sockaddr *)&client_addr,&sin_size)) == -1)
			{
				perror("accept");
				continue;
			}

			if(conn_amount <= BACKLOG)//添加到队列
			{
				client[++conn_amount] = newfd;
				printf("客户端(%d)连接  %s:%d\n",conn_amount,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

				if(newfd > maxsock)
				maxsock = newfd;
			}

			else
			{
				printf("达到最大连接数，退出");
				close(newfd);
			}
	
		}	
	}
	pthread_exit(NULL);	
}

void recvthread_create()
{
	bzero(&recvthread,sizeof(recvthread));
	if(pthread_create(&recvthread, NULL,looprecv,NULL) != 0)
		printf("线程recv创建失败!\n");
	else
		printf("线程1创建成功!\n");
}
/*******************************************************************/
void closecn()
{
	for(i = 0;i < BACKLOG;i++)//关闭所有连接
	{
		if(client[i] != 0)
		{
			close(client[i]);
		}
	}

}
/*******************************************************************/

/****************************loop***********************************/
void loop()
{
while(1)
{
	if(symbol)
		switch(BUF->cmd)
		{

			case 1:
				{
					clientlogin();
					symbol = 0;
					break;
				}
			case 2:
				{
					addfriend();
					symbol = 0;
					break;
				}
			case 3:
				{
					chat();
					symbol = 0;
					break;
				}
			case 4:
				{
					groupchat();
					symbol = 0;
					break;
				}
		}
}
}
/*******************************************************************/
int main()
{
	userinfo();

	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&YES,sizeof(int)) == -1)
	{
		perror("setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(IP);//INADDR_ANY
	bzero(&(server_addr.sin_zero),8);
	sockfd = socket(AF_INET,SOCK_STREAM,0); 
	if(bind(sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1)
	{
		perror("bind");
		exit(1);
	}
	if(listen(sockfd,BACKLOG) == -1)
	{
		perror("listen");
		exit(1);
	}
	else
		printf("监听端口：%d\n",PORT);

	pthread_mutex_t mut;
	pthread_mutex_init(&mut,NULL);
	recvthread_create();
	loop();
	for(i = 0;i < USER;i++)
		free_login_info(&LOGIN0[i]);
	closecn();

	return 0;
}
