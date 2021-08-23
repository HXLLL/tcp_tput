#include "tcp_tput.h"

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

int conn_fd, sockfd;
char buffer[BLK_SIZE]; 

void sigint_handler() {
    close(conn_fd);
    close(sockfd);
    printf("server shutting down\n");
    exit(0);
}

int main() {
    
    // signal(SIGINT, sigint_handler);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int reuse_addr_value = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr_value, 4);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("bind failed");
        printf("Error: %s\n", strerror(errno));
        return -1;
    }

    puts("Successfully binded");
    
    listen(sockfd, 1);

    long long cnt = 0;

    struct sockaddr_in client_addr;
    while (1) {
        char s[20]; int client_size;
        client_size = sizeof(client_addr);
        if ((conn_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_size)) < 0) {
            puts("failed to accept connection");
            printf("Error: %s\n", strerror(errno));
            continue;
        };
        inet_ntop(AF_INET, &client_addr.sin_addr, s, sizeof(client_addr));
        printf("client from %s:%d connected\n", s, ntohs(client_addr.sin_port));

        int len;
        while (cnt < BLK_CNT * BLK_SIZE) {
            len = read(conn_fd, buffer, BLK_SIZE);
            cnt += len;
        }
        write(conn_fd, "OK", 2);
        printf("transmission done\n");
        close(conn_fd);
        printf("connection done\n");
    }
    
}