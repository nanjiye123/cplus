//通用函数封装
#include "tcp_net_socket.h"

//*************************************************************************************
int tcp_init(const char* ip, int port)   //用于初始化操作
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);//首先创建一个socket，生成套接口描述符
    if(sfd == -1)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serveraddr; //存放ip地址和端口号
    memset(&serveraddr, 0, sizeof(struct sockaddr));
    serveraddr.sin_family = AF_INET; //ipv4协议
    serveraddr.sin_port = htons(port); //主机序转化网络序
    serveraddr.sin_addr.s_addr = inet_addr(ip);//将十进制转化网络序二进制

    if(bind(sfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) == -1)
    {//将新的socket与制定的ip、port绑定
        perror("bind");
        close(sfd);
        exit(-1);
    }
    if(listen(sfd, 10) == -1)//监听它，并设置其允许最大的连接数为10个
    {
        perror("listen");
        close(sfd);
        exit(-1);
    }
    return sfd;
}

//*************************************************************************************

int tcp_accept(int sfd)   //用于服务端的接收
{
    struct sockaddr_in clientaddr; //客户端地址
    memset(&clientaddr, 0, sizeof(struct sockaddr));
    int addrlen = sizeof(struct sockaddr);

    int new_fd = accept(sfd, (struct sockaddr*)&clientaddr, &addrlen);
    //sfd接受客户端连接，并创建新的socket为new_fd，将请求连接的客户端的ip、port保存在结构体clientaddr中
    if(new_fd == -1)
    {
        perror("accept");
        close(sfd);
        exit(-1);
    }

    printf("%s %d success connect...\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
    //二进制转十进制地址，网络序转主机序
    return new_fd;
}

//*************************************************************************************

int tcp_connect(const char* ip, int port)   //用于客户端的连接
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);//向系统注册申请新的socket
    if(sfd == -1)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(struct sockaddr));
    serveraddr.sin_family = AF_INET;            //ipv4协议
    serveraddr.sin_port = htons(port);          //主机序转化网络序
    serveraddr.sin_addr.s_addr = inet_addr(ip); //将十进制转化网络序二进制
    if(connect(sfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) == -1)
    {//将sfd连接至制定的服务器网络地址serveraddr
        perror("connect");
        close(sfd);
        exit(-1);
    }
    return sfd;
}
 
//*************************************************************************************

void signalhandler(void)   //用于信号处理，让服务端在按下Ctrl+c或Ctrl+\的时候不会退出
{
    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet,SIGINT);
    sigaddset(&sigSet,SIGQUIT);
    sigprocmask(SIG_BLOCK,&sigSet,NULL);
}

//*************************************************************************************