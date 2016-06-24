//udp接收hello，发送文件名，发送文件大小，再发送文件
//  ./udp_sever filename
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 1024
#define DEFAULT_SVR_PORT 2828 //默认端口号
#define FILE_MAX_LEN 64       //文件名最大长度
char filename[FILE_MAX_LEN+1];

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("need a filename without path\n");
        exit(1);
    }
    strncpy(filename, argv[1], FILE_MAX_LEN);//将输入的文件名拷贝到变量中

    //==================================================================
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);//步骤1：建立udp套接字
    if(sfd == -1)
    {
        perror("socket");
        exit(-1);
    }
    
    //==================================================================
    //步骤2：初始化结构体，并绑定2828端口
    unsigned short port = DEFAULT_SVR_PORT;
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET; /* ipv4 */
    saddr.sin_port = htons(port); /* 设置侦听端口是 2828 , 用htons转成网络序*/
    saddr.sin_addr.s_addr = INADDR_ANY; /* INADDR_ANY来表示任意IP地址可能其通讯 */

    //==================================================================
    //步骤3:绑定套接口,把socket队列与端口关联起来.
    if(bind(sfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        close(sfd);
        exit(-1);
    }
 
    printf("wait client\n");
    //==================================================================
    char buf[BUFSIZE] = {0};
    struct sockaddr_in fromaddr;
    bzero(&fromaddr, sizeof(fromaddr));
    int fromaddrlen = sizeof(struct sockaddr);
    if(recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&fromaddr, &fromaddrlen) == -1)
    {
        perror("recvfrom");
        close(sfd);
        exit(-1);
    }
    printf("receive from %s %d,the message is:%s\n", inet_ntoa(fromaddr.sin_addr), ntohs(fromaddr.sin_port), buf);//接收hello
 
    //==================================================================
    sendto(sfd, filename, sizeof(filename), 0, (struct sockaddr*)&fromaddr, sizeof(struct sockaddr));//发送文件名

    //==================================================================
    FILE* fp = fopen(filename,"rb");
    struct stat st;  //用于获取文件内容的大小
    stat(filename, &st);
    int filelen = st.st_size;
    printf("send file total size: %d\n", filelen);
    sendto(sfd, (void*)&filelen, sizeof(int), 0, (struct sockaddr*)&fromaddr, sizeof(struct sockaddr));//发送文件大小

    printf("begin send.....\n");
    while(!feof(fp))   //表示没有到文件尾
    {
        int len = fread(buf,1,sizeof(buf),fp);
        printf("send size: %d\n", len);
        sendto(sfd, buf, len, 0, (struct sockaddr*)&fromaddr, sizeof(struct sockaddr));
    }
    printf("finish!\n");

    fclose(fp);
    close(sfd);
    return 0;
}