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

	int sockfd,newfd,numbytes,len,i;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buf[MAXDATESIZE],msg[MAXDATESIZE];
	socklen_t sin_size;
	int YES = 1;

	fd_set fdsr;
	int maxsock,conn_amount,sel;
	int client[BACKLOG];
	struct timeval timev;

void looprecv()
{
	

	conn_amount = 0;
	sin_size = sizeof(client_addr);
	maxsock = sockfd;
	while(1)
	{
		timev.tv_sec = 30;//设置超时
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
		
		sel = select(maxsock+1,&fdsr,NULL,NULL,NULL);//&timev);
		if(sel < 0)
		{
			perror("select");
			break;
		}
		else if(sel == 0)
		{
			printf("超时\n");
			continue;
		}

		for(i = 1;i <= conn_amount;i++)//检测所有套接字
		{			
			if(FD_ISSET(client[i],&fdsr))
			{
				bzero(buf,MAXDATESIZE);
				if((numbytes = recv(client[i],buf,sizeof(buf),0)) == -1)
				{
					perror("recv");
				
				}
				if(numbytes == 0)
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
				{
					if(numbytes < MAXDATESIZE)
					buf[numbytes] = '\0';
					printf("客户端(%d)：%s\n",i,buf);
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
				send(newfd,"bye",4,0);
				close(newfd);
				break;
			}
		
		}		
	}	

}
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
