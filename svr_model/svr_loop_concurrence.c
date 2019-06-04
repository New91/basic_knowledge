/*
 * 并行处理客户端的请求
 * */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define BUFFLEN 1024
#define SERVER_PORT 8888
#define BACKLOG 5
#define PIDNUMB 2

static void handle_connect(int s)
{
    struct sockaddr_in from;/* 客户端地址 */
    socklen_t len = sizeof(from);
    int n = 0;
    char buff[BUFFLEN];
    time_t now;

    // 主处理过程
    while(1)
    {
        memset(buff, 0, BUFFLEN);
        // 接受客户端连接
        n = recvfrom(s, buff, BUFFLEN, 0,(struct sockaddr*)&from, &len);
        // 接收发送发数据
        if(n>0 && !strncmp(buff,"TIME", 4))
        {
            memset(buff, 0, BUFFLEN);
            now = time(NULL);
            sprintf(buff, "%24s\r\n", ctime(&now));
            sendto(s, buff, strlen(buff), 0, (struct sockaddr*)&from, len);
        }
    }
}

void sig_int(int num)
{
    exit(1);
}

int main(int argc, char* argv[])
{
    int s_s;
    struct sockaddr_in local;

    signal(SIGINT, sig_int);
    //建立UDP套接字
    s_s = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(SERVER_PORT);

    bind(s_s,(struct sockaddr*)&local, sizeof(local));

    /* 处理客户端连接 */
    pid_t pid[PIDNUMB];
    int i = 0;
    for(i=0;i<PIDNUMB;i++)
    {
        pid[i] = fork();
        if(pid[i] == 0)
            handle_connect(s_s);
    }
    //主处理过程
    while(1)
    {
    };

    return 0;
}
