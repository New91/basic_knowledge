#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT_SERV 8888
#define BUFF_LEN 256
void udp_serv_echo(int s, struct sockaddr* clt)
{
    int n;
    char buff[BUFF_LEN];
    socklen_t len;
    while(1)
    {
        len = sizeof(*clt);
        n = recvfrom(s, buff, BUFF_LEN, 0, clt, &len);
        sendto(s, buff, n, 0, clt, len);
    }
}

int main(int argc, char* argv[])
{
    int s;
    struct sockaddr_in addr_srv;
    struct sockaddr_in addr_clt;

    s = socket(AF_INET, SOCK_DGRAM, 0); // 建立数据包套接字

    memset(&addr_srv, 0, sizeof(addr_srv));
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_srv.sin_port = htons(PORT_SERV);

    bind(s, (struct sockaddr*)&addr_srv, sizeof(addr_srv));

    udp_serv_echo(s, (struct sockaddr*)&addr_clt);
    return 0;
}
