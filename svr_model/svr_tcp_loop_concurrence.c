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

void sig_int(int num)
{
    exit(1);
}


static void handle_connect(int s_s)
{
    int s_c;
    struct sockaddr_in from;
    socklen_t len = sizeof(from);

    while(1)
    {
        s_c = accept(s_s, (struct sockaddr*)&from, &len);
        time_t now;
        char buff[BUFFLEN];
        int n = 0;
        memset(buff, 0, BUFFLEN);
        n = recv(s_c, buff, BUFFLEN, 0);
        if(n > 0  && !strncmp(buff, "TIME", 4))
        {
            memset(buff, 0, BUFFLEN);
            now = time(NULL);
            sprintf(buff, "%24s\r\n", ctime(&now));
            send(s_c, buff, strlen(buff), 0);
        }
        close(s_c);
    }
}

int main(int argc, char* argv[])
{
    int s_s;
    struct sockaddr_in local;
    signal(SIGINT, sig_int);

    //建立TCP套接字
    s_s = socket(AF_INET, SOCK_STREAM, 0);

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    local.sin_port = htons(SERVER_PORT);

    bind(s_s,(struct sockaddr*)&local, sizeof(local));
    listen(s_s, BACKLOG);//侦听

    // 处理客户端连接
    pid_t pid[PIDNUMB];
    int i = 0;
    for(i=0;i<PIDNUMB;i++)
    {
        pid[i] = fork();
        if(pid[i] == 0)
            handle_connect(s_s);
    }
    //主处理过程
    while(1){};

    close(s_s);

    return 0;
}
