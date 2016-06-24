//TCP 文件服务器演示代码，发送文件
// 编译时加上-lpthread
//  ./file_sever filename
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
 
#define DEFAULT_SVR_PORT 2828 //默认端口号
#define FILE_MAX_LEN 64       //文件名最大长度
char filename[FILE_MAX_LEN+1];
 
static void * handle_client(void * arg)
{
       int sock = (int)arg;
       char buff[1024];
       int len_read, len_send;
       printf("begin send\n");
       FILE* file = fopen(filename,"r");
       if(file == NULL)
       {
              close(sock);
              exit;
       }
       //发文件名
       if(send(sock,filename,FILE_MAX_LEN,0) == -1)
       {
              perror("send file name\n");
              goto EXIT_THREAD;
       }
       printf("begin send file %s....\n",filename);
       //发文件内容
       while(!feof(file))
       {
              len_read = fread(buff,1,sizeof(buff),file);
              len_send = send(sock,buff,len_read,0);
              printf("server read %s, read len %d, send len %d\n", filename,len_read,len_send);
              //puts(buff);

              //int aaa = send(sock, "hello world", 12, 0);
              if(len_send < 0)
              {
                     perror("send file:");
                     goto EXIT_THREAD;
              }
              
       }
       printf("finish!\n");
       EXIT_THREAD:
       if(file)
              fclose(file);
       close(sock);
       printf("exit handle_client\n");
}
        
int main(int argc,char * argv[])
{
       int sockfd,new_fd;
       struct sockaddr_in my_addr; //定义两个ipv4 地址
       struct sockaddr_in their_addr;
       int sin_size,numbytes;
       pthread_t cli_thread;
       unsigned short port;
      
       if(argc < 2)
       {
              printf("need a filename without path\n");
              exit;
       }
       strncpy(filename, argv[1], FILE_MAX_LEN);//将输入的文件名拷贝到变量中
       port = DEFAULT_SVR_PORT;//默认端口号赋值
       if(argc >= 3)
       {
              port = (unsigned short)atoi(argv[2]);//如果输入了端口号
       }

       //==============================================================
       //第一步:建立TCP套接字 Socket
       // AF_INET --> ip通讯
       //SOCK_STREAM -->TCP
       if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
       {
              perror("socket");
              exit(-1);
       }

       //==============================================================
       //第二步:设置侦听端口
       //初始化结构体，并绑定2828端口
       memset(&my_addr,0,sizeof(struct sockaddr));
       my_addr.sin_family = AF_INET;     /* ipv4 */
       my_addr.sin_port = htons(port);   /* 设置侦听端口是 2828 , 用htons转成网络序*/
       my_addr.sin_addr.s_addr = INADDR_ANY;/* INADDR_ANY来表示任意IP地址可能其通讯 */
       //bzero(&(my_addr.sin_zero),8);
       
       //==============================================================
       //第三步:绑定套接口,把socket队列与端口关联起来.
       if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr)) == -1)
       {
              perror("bind");
              goto EXIT_MAIN;
       }

       //===============================================================
       //第四步:开始在2828端口侦听,是否有客户端发来联接
       if(listen(sockfd,10) == -1)
       {
              perror("listen");
              goto EXIT_MAIN;
       }
       printf("#@ listen port %d\n",port);

       //================================================================
       //第五步:循环与客户端通讯
      
	   //char ipsource[32] = {'\0'};
	while(1)
       {
		sin_size = sizeof(struct sockaddr);
		      //memset(&their_addr, 0, sizeof(struct sockaddr));
              printf("server waiting...\n");
              //如果有客户端建立连接，将产生一个全新的套接字 new_fd,专门用于跟这个客户端通信
              if((new_fd = accept(sockfd,(struct sockaddr *)&their_addr,&sin_size)) == -1)
              {
                     perror("accept:");
                     goto EXIT_MAIN;
              }
		//memcpy(ipsource, inet_ntoa(their_addr.sin_addr), strlen(inet_ntoa(their_addr.sin_addr)) );
              //printf("---client (ip=%s:port=%d) request \n", ipsource, ntohs(their_addr.sin_port));
			  
			 /* 
			  char buf[512] = {0};
			  if(recv(new_fd, buf, sizeof(buf), 0) == -1)
			  {
					perror("recv");
					exit(1);
			  }
			  puts(buf);*/
              
			  
			  //生成一个线程来完成和客户端的会话，父进程继续监听
              pthread_create(&cli_thread,NULL,handle_client,(void *)new_fd);
              pthread_join(cli_thread, NULL);
       }//while(1)end
       
       //================================================================
       //第六步:关闭socket
       EXIT_MAIN:
       close(sockfd);
       printf("exit main\n");
       return 0;
}


/* 读写大容量的文件时，通过下面的方法效率很高
ssize_t readn(int fd,char *buf,int size)//读大量内容
{
char *pbuf=buf;
int total ,nread;
for(total = 0; total < size; )
{
nread=read(fd,pbuf,size-total);
if(nread==0)
              return total;
              if(nread == -1)
              {
              if(errno == EINTR)
              continue;
              else
                     return -1;
              }
              total+= nread;
              pbuf+=nread;
       }
       return total;
}*/
