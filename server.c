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
	struct login
	{
		int loginname;
		char passwd[LOGINNAME];
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


	struct userinfo
	{
		int number;
		char name[USERNAME];
		char sex[USERSEX];
		char age[USERAGE];
	};

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
	int sockfd,newfd,len,i,j,k,l,m,n,tempnumber=0;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int YES = 1;

	fd_set fdsr,chatfdsr;
	pthread_t chatthread[2],addfriendthread,recvthread;
	int maxsock,conn_amount,sel;
	int client[BACKLOG],numbytes[USER];
	char buf1[MAXDATESIZE];
	
	struct timeval timev;
	struct login LOGIN;
	struct login LOGIN0[USER];
	struct buffer BUF[USER];
	struct massage MSG;
	struct userinfo user[USER];
	struct myfrd friend[USER];
	struct selectclient cltfd[USER];
/*******************************************************************/

/****************************USERINFO*******************************/
void userinfo()
{
	LOGIN0[0].loginname = 11;
	strcpy(LOGIN0[0].passwd,"123456");
	LOGIN0[1].loginname = 12;
	strcpy(LOGIN0[1].passwd,"123456");
	LOGIN0[2].loginname = 13;
	strcpy(LOGIN0[2].passwd,"123456");
	LOGIN0[3].loginname = 14;
	strcpy(LOGIN0[3].passwd,"123456");
	LOGIN0[4].loginname = 15;
	strcpy(LOGIN0[4].passwd,"123456");
	LOGIN0[5].loginname = 16;
	strcpy(LOGIN0[5].passwd,"123456");
	LOGIN0[6].loginname = 17;
	strcpy(LOGIN0[6].passwd,"123456");
	LOGIN0[7].loginname = 18;
	strcpy(LOGIN0[7].passwd,"123456");
	LOGIN0[8].loginname = 19;
	strcpy(LOGIN0[8].passwd,"123456");
	LOGIN0[9].loginname = 20;
	strcpy(LOGIN0[9].passwd,"123456");
	
	user[0].number = 11;
	strcpy(user[0].name,"admin1");
	strcpy(user[0].sex,"男");
	strcpy(user[0].age,"20");

	user[1].number = 12;	
	strcpy(user[1].name,"admin2");
	strcpy(user[1].sex,"女");
	strcpy(user[1].age,"21");

	user[2].number = 13;
	strcpy(user[2].name,"admin3");
	strcpy(user[2].sex,"男");
	strcpy(user[2].age,"22");

	user[3].number = 14;
	strcpy(user[3].name,"admin2");
	strcpy(user[3].sex,"男");
	strcpy(user[3].age,"23");

	user[4].number = 15;
	strcpy(user[4].name,"admin5");
	strcpy(user[4].sex,"女");
	strcpy(user[4].age,"24");

	user[5].number = 16;
	strcpy(user[5].name,"admin6");
	strcpy(user[5].sex,"男");
	strcpy(user[5].age,"22");

	user[6].number = 17;	
	strcpy(user[6].name,"admin7");
	strcpy(user[6].sex,"女");
	strcpy(user[6].age,"26");

	user[7].number = 18;
	strcpy(user[7].name,"admin8");
	strcpy(user[7].sex,"男");
	strcpy(user[7].age,"20");

	user[8].number = 19;
	strcpy(user[8].name,"admin9");
	strcpy(user[8].sex,"男");
	strcpy(user[8].age,"23");

	user[9].number = 20;
	strcpy(user[9].name,"admin10");
	strcpy(user[9].sex,"女");
	strcpy(user[9].age,"24");

	friend[0].numfrd = 11;
	friend[1].numfrd = 12;
	friend[2].numfrd = 13;
	friend[3].numfrd = 14;
	friend[4].numfrd = 15;
	friend[5].numfrd = 16;
	friend[6].numfrd = 17;
	friend[7].numfrd = 18;
	friend[8].numfrd = 19;
	friend[9].numfrd = 20;
	for(j = 0;j < USER;j++)
	{
		BUF[j].head.cmd = 0;
	for(k = 0;k < USER;k++)
		friend[j].list[k].numlist = 0;
	}
	
	cltfd[0].clientacc = 11;
	cltfd[1].clientacc = 12;
	cltfd[2].clientacc = 13;
	cltfd[3].clientacc = 14;
	cltfd[4].clientacc = 15;
	cltfd[5].clientacc = 16;
	cltfd[6].clientacc = 17;
	cltfd[7].clientacc = 18;
	cltfd[8].clientacc = 19;
	cltfd[9].clientacc = 20;
	
	MSG.head.number = 0;

}
/*******************************************************************/

/*****************************LOGIN*********************************/
void clientlogin()
{
	memcpy(&LOGIN,BUF[j].buf,sizeof(struct login));
	for(k = 0;k < USER;k++)
	{
		if((LOGIN.loginname == LOGIN0[k].loginname) && (bcmp(LOGIN.passwd,LOGIN0[k].passwd,6) == 0))
		{
			bzero(&MSG,sizeof(struct massage));
			MSG.head.number = 0;MSG.head.cmd = 1;
			memcpy(MSG.msg,"Login success!",14);
			MSG.msg[14] = '\0';
			if(send(newfd,&MSG,sizeof(struct massage),0) == -1)
			{
				perror("send");
				
			}
			else
			for(l = 0;l < USER;l++)
			{
				if(LOGIN.loginname == cltfd[l].clientacc)
				{
					cltfd[l].clientfd = client[j];
					printf("客户端%d已登录%d！\n",cltfd[l].clientacc,cltfd[l].clientfd);
					break;
				}
			}
			break;
		}
	}
	if(k == USER)
	{
		bzero(&MSG,sizeof(struct massage));
		MSG.head.number = 0;MSG.head.cmd = 1;
		memcpy(MSG.msg,"Infor error",12);
		MSG.msg[12] = '\0';
		if(send(newfd,&MSG,sizeof(struct buffer),0) == -1)
		{
			perror("send");
		}
		printf("客户端(%d) 验证失败%d\n",j,client[j]);
	}
}
/*******************************************************************/	

/************************addfriend**********************************/
void addfriend()
{
	MSG.head.cmd = 2;
	while(1)
	{	
		tempnumber = (BUF[j].buf[0] - '0')*10+(BUF[j].buf[1] - '0');
		for(n = 0;n < USER;n++)
		{
			if(tempnumber == cltfd[n].clientacc)
				break;
		}
		if(n < USER)
		{
		for(n = 0;n < USER;n++)
		{
			if(tempnumber == user[n].number)
			{
			for(k = 0;k < USER;k++)
			{
				if(friend[k].numfrd == BUF[j].head.number)
				{
				for(l = 0;l < USER;l++)
				{
					if(friend[k].list[l].numlist == tempnumber)
					{
						memcpy(MSG.msg,"already in list!",16);
						MSG.msg[16] = '\0';
						if(send(client[j],&MSG,sizeof(struct massage),0) == -1)
						{
							perror("send");
						}
						printf("already in list!\n");
						break;
					}
					else if(friend[k].list[l].numlist == 0)
					{
						friend[k].list[l].numlist = tempnumber;
						memcpy(MSG.msg,"add success!",12);
						MSG.msg[12] = '\0';
						if(send(client[j],&MSG,sizeof(struct massage),0) == -1)
						{
							perror("send");
						}
						for(m = 0;m < USER;m++)
						{
							if(cltfd[m].clientacc == tempnumber)
							{
								memcpy(MSG.msg,"already add by friend!",22);
								MSG.msg[22] = '\0';
								if(send(cltfd[m].clientfd,&MSG,sizeof(struct massage),0) == -1)
								{
									perror("send");
								}
								else
									break;
							}
						}
						break;
					}
					else
					{
						if(l == (USER-1))
						{
						memcpy(MSG.msg,"not found!",10);
						MSG.msg[10] = '\0';
						if(send(client[j],&MSG,sizeof(struct massage),0))
						{
							perror("send");
						}
						else
						{
							break;
						}
						}
					}
				}
				break;
				}
			}
			break;
			}
			else if(n == (USER-1))
			{
				memcpy(MSG.msg,"No user!",8);
				MSG.msg[8] = '\0';
				if(send(client[i],&MSG,sizeof(struct massage),0))
				{
					perror("send");
				}
				else
					break;
			}
		}
		break;
		}
	}
}

/*******************************************************************/

/***************************friendlist*****************************/
void friendlist()
{
	for(n = 0;n < USER;n++)
	{
		if(BUF[j].head.number == friend[n].numfrd)
		{
			MSG.head.cmd = 5;
			bzero(MSG.msg,MAXDATESIZE);
			memcpy(MSG.msg,friend[n].list,sizeof(struct frdlist)*USER);
			MSG.msg[sizeof(struct frdlist)*USER] = '\0';
			if((send(client[j],&MSG,sizeof(struct massage),0)) == -1)
			{
				perror("send");
			}
			for(k = 0,l = 0;k < USER;k++)
			{
				if(friend[n].list[k].numlist != 0)
					printf("friend%d ：%d\n",++l,friend[n].list[k].numlist);
			}
			if(l == 0)
			{
				bzero(MSG.msg,MAXDATESIZE);
				MSG.head.cmd = 0;
				memcpy(MSG.msg,"NO friend!",10);
				MSG.msg[10] = '\0';
				if((send(client[j],&MSG,sizeof(struct massage),0)) == -1)
				{
					perror("send");
				}	
			}
			break;
		}
		
	}
}
/*******************************************************************/

/****************************chat***********************************/
void chat()
{
	for(k = 0;k < USER;k++)
	{
		if((BUF[j].head.friend == cltfd[k].clientacc) && cltfd[k].clientfd > 2)
		{
			if(send(cltfd[k].clientfd,&BUF[j],sizeof(struct buffer),0) == -1)
			{
				perror("send");
			}
			else
			{
				break;
			}
		}
		else if(k == USER)
		{
			bzero(&MSG,sizeof(struct massage));
			MSG.head.cmd = 3;MSG.head.number = 0;MSG.head.friend = BUF[j].head.friend;
			memcpy(MSG.msg,"Not friend or friend not online",31);
			MSG.msg[31] = '\0';
			if((send(client[j],&MSG,sizeof(struct massage),0)) == -1)
			{
				perror("send");
			}
		}
	}
}

/*******************************************************************/

/***************************groupchat*******************************/
void groupchat()
{
	for(k = 0;k <= conn_amount;k++)
	{
		if(client[k] > 0)
		{
			if(send(client[k],&BUF[j],sizeof(struct buffer),0) == -1)
			{
				perror("send");
			}	
		}
		else if(k == USER)
		{
			bzero(&MSG,sizeof(struct massage));
			MSG.head.cmd = 4;MSG.head.number = 0;MSG.head.friend = BUF[j].head.friend;
			memcpy(MSG.msg,"Not user online!",16);
			MSG.msg[16] = '\0';
			if((send(client[j],&MSG,sizeof(struct massage),0)) == -1)
			{
				perror("send");
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
				bzero(&BUF[i],sizeof(struct buffer));
				if((numbytes[i] = recv(client[i],&BUF[i],sizeof(struct buffer),0)) == -1)
				{
					perror("recv");
				}
				else if(numbytes[i] == 0)
				{
					printf("客户端(%d) 关闭\n",i);
					close(client[i]);
					FD_CLR(client[i],&fdsr);
					for(;i<conn_amount;i++)
					{
						client[i] = client[i+1];
					}
//					client[i] = 0;
					conn_amount--;
				}
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
//				send(newfd,"bye",4,0);
				close(newfd);
//				break;
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
for(j = 0;j <= conn_amount;j++)
	if((strlen(BUF[j].buf) > 0) && (client[j] > 0))
	{
		switch(BUF[j].head.cmd)
		{

			case 1:
				{
					clientlogin();
					BUF[j].head.cmd = 0;
					break;
				}
			case 2:
				{
					addfriend();
					BUF[j].head.cmd = 0;
					break;
				}
			case 3:
				{
					chat();
					BUF[j].head.cmd = 0;
					break;
				}
			case 4:
				{
					groupchat();
					BUF[j].head.cmd = 0;
					break;
				}
			case 5:
				{
					friendlist();
					BUF[j].head.cmd = 0;
					break;
				}
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
	
	closecn();

	return 0;
}
