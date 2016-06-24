//服务器端
#include "tcp_net_socket.h"

int main(int argc, char* argv[])
{
       if(argc < 3)
       {
              printf("usage:./servertcp  ip  port\n");
              exit(-1);
       }

       signalhandler();
       
       int sfd = tcp_init(argv[1], atoi(argv[2])); //socket初始化，返回套接口描述符
       //int sfd = tcp_init("192.168.56.101", 8888);
       while(1)  //用while循环表示可以与多个客户端接收和发送，但仍是阻塞模式的
       {
              int cfd = tcp_accept(sfd); //接受客户端的连接，返回服务器端新套接字
              char buf[512] = {0};
              if(recv(cfd, buf, sizeof(buf), 0) == -1)//用新套接字接收客户端数据，存于buf中
              {
                     perror("recv");
                     close(cfd);
                     close(sfd);
                     exit(-1);
              }
              puts(buf);
              if(send(cfd, "hello world", 12, 0) == -1)//用新套接字发送数据给客户端
              {
                     perror("send");
                     close(cfd);
                     close(sfd);
                     exit(-1);
              }
              close(cfd);
       }
       close(sfd);
}