#include "tcp_tput.h"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main() {
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

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

    int cnt = 0;

    struct sockaddr_in client_addr;
    while (1) {
        char s[20]; int client_size;
        int conn_fd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_size);
        inet_ntop(AF_INET, &client_addr, s, sizeof(client_addr));
        printf("client from %s:%d connected\n", s, client_addr.sin_port);

        char buffer[1024]; size_t len;
        while (len = read(conn_fd, buffer, 1024)) {
            buffer[len] = 0;
            puts(buffer);
        }
        write(conn_fd, "OK", 2);
        close(conn_fd);
    }
}