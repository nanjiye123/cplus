//客户端
#include "tcp_net_socket.h"

int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        printf("usage:./clienttcp  ip  port\n");
        exit(-1);
    }

    int sfd = tcp_connect(argv[1],atoi(argv[2])); //用于客户端的连接，返回客户端套接字
    char buf[512] = {0};
    send(sfd, "hello", 6, 0);     //向服务端发送数据
    recv(sfd, buf, sizeof(buf), 0); //从服务端接收数据
    puts(buf);
    close(sfd);
}