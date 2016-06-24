//tcp文件传输客户端，接收文件
//  ./file_client server_ip
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
 
#define FILE_MAX_LEN 64
#define DEFAULT_SVR_PORT 2828
 
int main(int argc,char * argv[])
{
       int sockfd,numbytes;
       char buf[1024]={0}, filename[FILE_MAX_LEN+1];
       char ip_addr[64];
       struct hostent *he;
       struct sockaddr_in their_addr;
       int i = 0,len,total;
       unsigned short port;
       FILE * file = NULL;

       if(argc <2)
       {
              printf("need a server ip \n");
              exit;
       }    
       strncpy(ip_addr,argv[1],sizeof(ip_addr)); //复制输入的ip地址到变量中
       port = DEFAULT_SVR_PORT; //默认端口号
       if(argc >=3)
       {
              port = (unsigned short)atoi(argv[2]);
       }
       //做域名解析(DNS)
       //he = gethostbyname(argv[1]);
       
       //======================================================================
       //第一步:建立一个TCP套接字
       if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {
              perror("socket");
              exit(1);
       }

       //======================================================================
       //第二步:设置服务器地址和端口2828
       memset(&their_addr,0,sizeof(their_addr));
       their_addr.sin_family = AF_INET;
       their_addr.sin_port = htons(port);
       their_addr.sin_addr.s_addr = inet_addr(ip_addr);
       //their_addr.sin_addr = *((struct in_addr *)he->h_addr);
       //bzero(&(their_addr.sin_zero),8);
       printf("connect server %s:%d\n",ip_addr,port);

       //======================================================================
       /*第三步:用connect 和服务器建立连接 ,注意,这里没有使用本地端口,将由协议栈自动分配一个端口*/
       if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr))==-1){
              perror("connect");
              exit(1);
       }

      
	   // 接收文件名,为编程简单,假设前64字节固定是文件名,不足用0来增充 
       total = 0;
       while(total< FILE_MAX_LEN){
              //注意这里的接收buffer长度,始终是未接收文件名剩下的长度,
              len = recv(sockfd,filename+total,(FILE_MAX_LEN - total),0);
              if(len <= 0)
                     break;
              total += len ;
       }
       // 接收文件名出错 
       if(total != FILE_MAX_LEN){
              perror("failure file name");
              exit(-3);
       }    
       printf("recv file %s.....\n",filename);
       file = fopen(filename,"wb");
       //file = fopen("/home/hxy/abc.txt","wb");
       if(file == NULL)
       {
              printf("create file %s failure",filename);
              perror("create:");
              exit(-3);
       }
       //接收文件数据
       printf("recv begin\n");
       total = 0;
       while(1)
       {
              len = recv(sockfd,buf,sizeof(buf),0);
              if(len <= 0) //对方断开或出错时退出
              {
                     break;
              }
              printf("receive len: %d\n", len);
              total += len;
              fwrite(buf, 1, len, file);//写入本地文件
       }
       fclose(file);
       printf("recv file %s success total lenght %d\n",filename,total);
       //第六步:关闭socket
       close(sockfd);
}
       
/* 读写大容量的文件时，通过下面的方法效率很高
ssize_t writen(int fd, char *buf, int size)//写大量内容
{
       char *pbuf=buf;
       int total ,nwrite;
       for(total = 0; total < size; )
       {
       nwrite=write(fd,pbuf,size-total);
              if( nwrite <= 0 )
              {
                     if( nwrite == -1 && errno == EINTR )
                            continue;
                     else
                            return -1;
              }
              total += nwrite;
              pbuf += nwrite;
       }
       return total;
}
*/
