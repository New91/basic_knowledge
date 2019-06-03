/*
 * 循环服务器指的是对于客户端的请求和连接，
 * 服务端在处理完毕一个之后再处理下一个，
 * 即串行处理客户端的请求
 * */
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888

int main(int argc, char* argv[])
{
    int s;
    struct sockaddr_in local;
    struct sockaddr_in to;
    time_t now;
    char buff[BUFFLEN] = {0};
    int n = 0;
    socklen_t len = sizeof(to);
    //建立UDP套接字
    s = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(SERVER_PORT);

    bind(s,(struct sockaddr*)&local, sizeof(local));

    //主处理过程
    while(1)
    {
        memset(buff, 0, BUFFLEN);
        n = recvfrom(s, buff, BUFFLEN, 0, (struct sockaddr*)&to, &len);
        if(n > 0 && !strncmp(buff,"TIME", 4))
        {
            memset(buff,0,BUFFLEN);
            now = time(NULL);
            sprintf(buff, "%24s\r\n", ctime(&now));
            sendto(s, buff, strlen(buff),0, (struct sockaddr*)&to, len);//发送数据
        }
        printf("--\n");
    }

    close(s);

    return 0;
}
