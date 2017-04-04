#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error_die(const char *sc)
{
    perror(sc);
    exit(1);
}

int startSvr(u_short *port)
{
    int sockfd = 0;
    struct sockaddr_in myaddr;

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1)
    {
        error_die("socket");
    }

    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(*port);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in)) == -1)
    {
        error_die("bind");
    }

    if (listen(sockfd, 5) == -1)
    {
        error_die("listen");
    }

    return sockfd;
}

int main(int argc, const char *argv[])
{
    u_short port = 7700;
    int server_sock = -1;
    int client_sock = -1;

    server_sock = startSvr(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        client_sock = accept();
    }

    close(server_sock);
    
    return 0;
}
