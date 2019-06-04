
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>

#define PORT 8888
#define V 1

static struct iovec* vs = NULL;
static struct iovec* vc = NULL;

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
static int s;
void sig_proccess(int signo)
{
    printf("catch a exit signal\n");

#if V
    if(vc)
        free(vc);
    if(vs)
        free(vs);
#endif
    exit(0);
}
void sig_pipe(int signo)
{
    printf("catch a pipe signal\n");

#if V
    if(vc)
        free(vc);
    if(vs)
        free(vs);
#endif
    exit(0);
}

// 客户端的处理过程
#if V
void process_conn_client(int s)
{
    char buffer[30];
    ssize_t size = 0;

    struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
    vc = v; //挂接全局变量，便于释放

    //初始化消息
    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = v;
    msg.msg_iovlen = 30;
    msg.msg_flags = 0;


    v[0].iov_base = buffer;
    v[1].iov_base = buffer + 10;
    v[2].iov_base = buffer + 20;
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;

    int i = 0;
    while(1)
    {
        // 从标准输入中读取数据放到缓冲区buffer中
        size = read(0, v[0].iov_base, 10);
        if(size > 0){
            v[0].iov_len = size;
            //writev(s, v, 1);    // 发送给服务器
            sendmsg(s, &msg, 0);
            v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;

            // 从套接字中读取数据放到向量缓冲区中
            //size = readv(s, v, 3);                          // 从服务器读取数据
            size = recvmsg(s, &msg, 0);
            for(i=0;i<3;i++){
                if(v[i].iov_len > 0)
                    write(1, v[i].iov_base, v[i].iov_len);//写到标准输出
            }
           // memset(v,0, sizeof(struct iovec));
        }
    }
}
#else
void process_conn_client(int s)
{
    ssize_t size = 0;
    char buffer[1024];
    for(;;)
    {
        size = read(0, buffer, 1024);
        if(size > 0){
            send(s, buffer, size, 0);
            size = recv(s, buffer, 1024, 0);
            write(1, buffer, size);
        }
    }
}
#endif
int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr;
    int err;

    if(argc == 1){
        printf("please input server addr\n");
        return -1;
    }

    signal(SIGINT, sig_proccess);
    signal(SIGPIPE, sig_pipe);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        printf("socket error\n");
        return -1;
    }

    // 设置服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    // 将用户输入的字符串类型IP转换成整型
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));

    process_conn_client(s);
    return 0;
}

























