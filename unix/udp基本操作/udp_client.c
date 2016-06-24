//udp发送hello，接收文件名，接收文件大小，接收文件
//  ./udp_client serverip
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024
#define FILE_MAX_LEN 64
#define DEFAULT_SVR_PORT 2828

int main(int argc, char* argv[])
{
    char ip_addr[64];
    int  len, total;
    char filename[FILE_MAX_LEN+1];
    if(argc <2)
    {
        printf("need a server ip \n");
        exit;
    }
    strncpy(ip_addr,argv[1],sizeof(ip_addr)); //复制输入的ip地址到变量中

    //=================================================================
    //步骤1：建立udp套接字
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd == -1)
    {
        perror("socket");
        exit(-1);
    }
 
    //=================================================================
    //第二步:设置服务器地址和端口2828
    unsigned short port = DEFAULT_SVR_PORT;
    struct sockaddr_in toaddr;
    bzero(&toaddr, sizeof(toaddr));
    toaddr.sin_family = AF_INET;
    toaddr.sin_port = htons(port);
    toaddr.sin_addr.s_addr = inet_addr(ip_addr);
    sendto(sfd, "hello", 6, 0, (struct sockaddr*)&toaddr, sizeof(struct sockaddr));
 
    char buf[BUFSIZE] = {0};
    struct sockaddr_in fromaddr;
    bzero(&fromaddr, sizeof(fromaddr));
    int fromaddrlen = sizeof(struct sockaddr);
    int filelen = 0;   //用于保存文件长度
    
    
    //=================================================================
    // 接收文件名,为编程简单,假设前64字节固定是文件名,不足用0来增充 
    total = 0;
    while(total< FILE_MAX_LEN)
    {
        //注意这里的接收buffer长度,始终是未接收文件名剩下的长度,
        len = recvfrom(sfd, filename+total, (FILE_MAX_LEN - total), 0, (struct sockaddr*)&fromaddr, &fromaddrlen);
        if(len <= 0)
            break;
        total += len ;
    }
    // 接收文件名出错 
    if(total != FILE_MAX_LEN)
    {
        perror("failure file name");
        exit(-3);
    }
    printf("filename: %s\n", filename);
    FILE* fp = fopen(filename,"w+b");

    //接收文件的长度
    recvfrom(sfd, (void*)&filelen, sizeof(int), 0, (struct sockaddr*)&fromaddr, &fromaddrlen);
    printf("the length of file is %d\n",filelen);
    printf("reveive file.....\n");

    //===================================================================
    //接收文件的内容
    while(1)
    {
        int len = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&fromaddr, &fromaddrlen);
        printf("receive len: %d\n", len);
        if(len < BUFSIZE)
        {//如果接收的长度小于BUFSIZE，则表示最后一次接收，此时要用break退出循环
            fwrite(buf,sizeof(char),len,fp);
            break;
        }
        fwrite(buf,sizeof(char),len,fp);
    }
    printf("receive file finished!\n");
    close(sfd);
    return 0;
}