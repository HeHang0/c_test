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

#define PORT 13490
#define BACKLOG 5
#define IP "127.0.0.1"
#define MAXDATESIZE 100
#define LOGINNAME 16
#define LOGINPWD 16
#define NAME 3
#define USER 5
#define USERNAME 7
#define USERSEX 4
#define USERAGE 3
#define USERNUMBER 3
#define ACCOUNTSIZE 2
	
/******************************struct*******************************/
	struct login
	{
		char loginname[LOGINNAME];
		char passwd[LOGINNAME];
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


	struct userinfo
	{
		char name[USERNAME];
		char sex[USERSEX];
		char age[USERAGE];
		char number[USERNUMBER];
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
	struct selectclient
	{
		char clientacc[NAME];
		int clientfd;
	};
/*******************************************************************/
	
/******************************declar*******************************/
	int sockfd,newfd,numbytes,len,i,j,k,l,m;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int YES = 1;

	fd_set fdsr;
	int maxsock,conn_amount,sel;
	int client[BACKLOG];
	
	struct timeval timev;
	struct login LOGIN;
	struct login LOGIN0[USER];
	struct buffer BUF[USER],BUF1;
	struct massage MSG[USER],MSG1;
	struct userinfo user[USER];
	struct myfrd friend[USER];
	struct selectclient cltfd[USER];
/*******************************************************************/

/****************************USERINFO*******************************/
void userinfo()
{
	strcpy(LOGIN0[0].loginname,"01");
	strcpy(LOGIN0[0].passwd,"123456");
	strcpy(LOGIN0[1].loginname,"02");
	strcpy(LOGIN0[1].passwd,"123456");
	strcpy(LOGIN0[2].loginname,"03");
	strcpy(LOGIN0[2].passwd,"123456");
	strcpy(LOGIN0[3].loginname,"04");
	strcpy(LOGIN0[3].passwd,"123456");
	strcpy(LOGIN0[4].loginname,"05");
	strcpy(LOGIN0[4].passwd,"123456");
	
	strcpy(user[0].name,"admin1");
	strcpy(user[0].sex,"男");
	strcpy(user[0].age,"20");
	strcpy(user[0].number,"01");
	
	strcpy(user[1].name,"admin2");
	strcpy(user[1].sex,"女");
	strcpy(user[1].age,"21");
	strcpy(user[1].number,"02");

	strcpy(user[2].name,"admin3");
	strcpy(user[2].sex,"男");
	strcpy(user[2].age,"22");
	strcpy(user[2].number,"03");

	strcpy(user[3].name,"admin2");
	strcpy(user[3].sex,"男");
	strcpy(user[3].age,"23");
	strcpy(user[3].number,"04");

	strcpy(user[4].name,"admin5");
	strcpy(user[4].sex,"女");
	strcpy(user[4].age,"24");
	strcpy(user[4].number,"05");

	for(j = 0;j < USER;j++)
	{
		friend[j].numfrd[0] = 0;
		friend[j].numfrd[1] = j+1;
	}
	for(k = 0;k < USER;k++)
	{
	for(l = 0;l < USER;l++)
		strcpy(friend[k].list[l].numlist,"00");
	}
	strcpy(friend[0].numfrd,"01");
	strcpy(friend[1].numfrd,"02");
	strcpy(friend[2].numfrd,"03");
	strcpy(friend[3].numfrd,"04");
	strcpy(friend[4].numfrd,"05");
	
	strcpy(cltfd[0].clientacc,"01");
	strcpy(cltfd[1].clientacc,"02");
	strcpy(cltfd[2].clientacc,"03");
	strcpy(cltfd[3].clientacc,"04");
	strcpy(cltfd[4].clientacc,"05");
}
/*******************************************************************/

/*****************************LOGIN*********************************/
clientlogin()
{
	bzero(&(LOGIN),sizeof(struct login));
	if(recv(newfd,&(LOGIN),sizeof(struct login),0) == -1)
	{
		perror("recv");
		exit(1);
	}
	else
	{
		for(j = 0;j < USER;j++)
		{
			if((bcmp(LOGIN.loginname,LOGIN0[j].loginname,ACCOUNTSIZE) == 0) && (bcmp(LOGIN.passwd,LOGIN0[j].passwd,6) == 0))
			{
				send(newfd,"Login success!",14,0);
				for(k = 0;k < USER;k++)
				{
					if(bcmp(LOGIN.loginname,cltfd[k].clientacc,ACCOUNTSIZE) == 0)
					{
						cltfd[k].clientfd = newfd;
						printf("%s 已登录\n",cltfd[k].clientacc);
						break;
					}
				}
				break;
			}
			if(j == (USER-1))
			{
				send(newfd,"Infor error",12,0);
				printf("客户端(%d) 验证失败\n",i);
				close(client[i]);
				FD_CLR(client[i],&fdsr);
				for(;i<conn_amount;i++)
				{
					client[i] = client[i+1];
				}
//				client[i] = 0;
				conn_amount--;

			}
		}

		
	}
}
/*******************************************************************/	

/************************addfriend**********************************/
void addfriend()
{
	while(1)
	{
		if((numbytes = recv(client[i],&BUF[i],sizeof(struct buffer),0)) == -1)
		{
			perror("recv");
		}
		else if(numbytes > 0)
		{
		for(j = 0;j < USER;j++)
		{
			if(bcmp(BUF[i].buf,user[j].number,ACCOUNTSIZE) == 0)
			{
			for(k = 0;k < USER;k++)
			{
				if(bcmp(friend[k].numfrd,BUF[i].numbuf,NAME) == 0)
				{
				for(l = 0;l < USER;l++)
				{
					if(bcmp(friend[k].list[l].numlist,BUF[i].buf,NAME) == 0)
					{
						send(client[i],"already in list!",16,0);
						printf("already in list!\n");
						break;
					}
					else if(bcmp(friend[k].list[l].numlist,"00",NAME) == 0)
					{
						strcpy(friend[k].list[l].numlist,BUF[i].buf);
						send(client[i],"add success!",12,0);
						printf("add success!\n");
						for(m = 0;m < USER;m++)
						{
							if(bcmp(BUF[i].buf,cltfd[m].clientacc,ACCOUNTSIZE) == 0)
							{
								if(send(cltfd[m].clientfd,"already add!",12,0) == -1)
								{
									perror("send");
								}
								else
									printf("sendto%s success!%d\n",cltfd[m].clientacc,cltfd[m].clientfd);
							}
						}
						break;
					}
					else
					{
						if(l == (USER-1))
						{
						send(client[i],"not found!",10,0);
						printf("not found!\n");
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
}
/*******************************************************************/
/************************looprecv***********************************/
void looprecv()
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
		
		for(i = 1;i <= conn_amount;i++)
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

		for(i = 1;i <= conn_amount;i++)//检测所有套接字
		{			
			if(FD_ISSET(client[i],&fdsr))
			{
				bzero(&BUF[i],sizeof(struct buffer));

				if((numbytes = recv(client[i],&BUF[i],sizeof(struct buffer),0)) == -1)
				{
					perror("recv");
				}
				else if(numbytes == 0)
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
				else
					if(numbytes <= sizeof(struct buffer))
				{
					printf("客户端%s：%s\n",BUF[i].numbuf,BUF[i].buf);
					if(bcmp(BUF[i].buf,"add",3) == 0)
					{
					addfriend();
					}
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
				clientlogin();
			}

			else
			{
				printf("达到最大连接数，退出");
				send(newfd,"bye",4,0);
				close(newfd);
				break;
			}
	
		}		
	}	
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

	looprecv();
	
	closecn();

	return 0;
}