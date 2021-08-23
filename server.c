#include "tcp_tput.h"

#include <net/if.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int conn_fd[NUM_THREADS], sockfd[NUM_THREADS];
char buffer[BLK_SIZE]; 

void sigint_handler() {
    for (int i=0;i!=NUM_THREADS;++i) {
        close(conn_fd[i]);
        close(sockfd[i]);
    }
    printf("server shutting down\n");
    exit(0);
}

pthread_t workers[NUM_THREADS];

void *serve(void *conn_fd_p) {
    int conn_fd = *(int*)conn_fd_p;
    long long cnt = 0, len;

    while (cnt < BLK_CNT * BLK_SIZE)
    {
        len = read(conn_fd, buffer, BLK_SIZE);
        cnt += len;
    }
    write(conn_fd, "OK", 2);
    printf("transmission done\n");
    close(conn_fd);
    printf("connection done\n");
}

int main() {

    signal(SIGINT, sigint_handler);

    for (int i=0;i!=NUM_THREADS;++i) {
        sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        int reuse_addr_value = 1;
        setsockopt(sockfd[i], SOL_SOCKET, SO_REUSEADDR, &reuse_addr_value, 4);

        struct ifreq ifr;

        memset(&ifr, 0, sizeof(ifr));
        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "enp94s0f0");
        if (setsockopt(sockfd[i], SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
            printf("Error: %s\n", strerror(errno));
            return -1;
        }

        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT + i);

        if (bind(sockfd[i], (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            puts("bind failed");
            printf("Error: %s\n", strerror(errno));
            return -1;
        }

        puts("Successfully binded");

        listen(sockfd[i], 1);
    }


    struct sockaddr_in client_addr;
    int worker_cnt=0;
    for (int i=0;i!=NUM_THREADS;++i) {
        char s[20];
        int client_size;
        client_size = sizeof(client_addr);
        if ((conn_fd[i] = accept(sockfd[i], (struct sockaddr *)&client_addr, (socklen_t *)&client_size)) < 0)
        {
            puts("failed to accept connection");
            printf("Error: %s\n", strerror(errno));
            continue;
        };
        inet_ntop(AF_INET, &client_addr.sin_addr, s, sizeof(client_addr));
        printf("client from %s:%d connected\n", s, ntohs(client_addr.sin_port));

        int *conn_fd_p = malloc(1);
        *conn_fd_p = conn_fd[i];
        pthread_create(&workers[worker_cnt++], NULL, serve, conn_fd_p);
    }
    
    for (int i=0;i!=NUM_THREADS;++i) {
        pthread_join(workers[i], NULL);
    }
}