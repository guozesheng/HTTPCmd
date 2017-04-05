#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

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

int readline(int sockfd, char *buf, int buflen)
{
    int n;
    int idx = 0;

    while (idx < buflen)
    {
        n = recv(sockfd, buf + idx, 1, 0);
        if (n <= 0 || *(buf + idx) == '\n')
        {
            break;
        }
        idx++;
    }

    if (idx < buflen - 1)
    {
        *(buf + idx) = '\0';
    }
    else
    {
        *(buf + buflen - 1) = '\0';
    }

    return idx;
}

void accept_request(void *pclient)
{
    int client = *(int *)pclient;
    char buf[256];
    int n;

    printf("in request sock=%d\n", client);
    while ((n=readline(client, buf, 256)) > 0)
    {
        printf("line=%d, %s\n", n, buf);
    }
}

int main(int argc, const char *argv[])
{
    u_short port = 7700;
    int server_sock = -1;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(struct sockaddr_in);
    pthread_t newthread;

    server_sock = startSvr(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (client_sock == -1)
        {
            error_die("accept");
        }

        printf("the client_sock = %d\n", client_sock);

        if (pthread_create(&newthread, NULL, (void *)accept_request, &client_sock) != 0)
        {
            perror("pthread_create");
        }
    }

    close(server_sock);
    
    return 0;
}
