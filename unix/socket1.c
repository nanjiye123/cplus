//十进制地址和网络字节序地址，转化函数
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main()
{
    char ip[] = "192.168.0.101";

    struct in_addr myaddr;//存ip地址，网络字节序

    int iRet = inet_aton(ip, &myaddr);//十进制转网络字节序
    printf("%x\n", myaddr.s_addr);

    printf("%x\n", inet_addr(ip));//十进制转网络字节序，同上

    iRet = inet_pton(AF_INET, ip, &myaddr);//十进制转网络字节序，选协议，同上
    printf("%x\n", myaddr.s_addr);

    myaddr.s_addr = 0xac100ac4;
    printf("%s\n", inet_ntoa(myaddr));//网络字节序转换十进制

    inet_ntop(AF_INET, &myaddr, ip, 16);//网络字节序转换十进制，同上
    puts(ip);
    return 0;
}