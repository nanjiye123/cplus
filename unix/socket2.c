//将www.baidu.com转化为ip地址
#include <netdb.h>
#include <sys/socket.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    char *ptr, **pptr;
    struct hostent *hptr;
    char str[32] = {'\0'};
    ptr = argv[1];  //输入参数，如www.baidu.com

    //调用gethostbyname()。结果存在hptr结构中
    if((hptr = gethostbyname(ptr)) == NULL)
    {
        printf(" gethostbyname error for host:%s\n", ptr);
        return 0;
    }

    printf("official hostname:%s\n",hptr->h_name);//将主机的规范名打出来
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
    {
        printf(" alias:%s\n",*pptr);//主机可能有多个别名，将所有别名分别打出来
    }
    
    switch(hptr->h_addrtype)//根据地址类型，将地址打出来
    {
    case AF_INET:
    case AF_INET6:
        pptr=hptr->h_addr_list;//主机的ip地址列表              
        for(; *pptr!=NULL; pptr++)//调用了inet_ntop()二进制转十进制
        {
            inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));
            printf(" address:%s\n", str);
        }
        inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str));
        printf(" first address: %s\n", str);
        break;
    default:
        printf("unknown address type\n");
        break;
    }
    return 0;
}