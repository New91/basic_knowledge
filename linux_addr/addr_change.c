#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* 端口转换的其他函数
#include <arpa/inet.h>
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
 * */
int main()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //地址结构的端口地址，网络字节序
    addr.sin_port = htons(8080);
    //IP, 将字符串的IP地址转化为网络字节序
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 把IP地址再转换成字符串，输出出来
    printf("Family:%u\n", addr.sin_family);
    printf("Port:%u\n", addr.sin_port);
    printf("Port:%u\n", ntohs(addr.sin_port));
    printf("IP:%s\n", inet_ntoa(addr.sin_addr));
    return 0;
}
