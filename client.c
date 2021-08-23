#include "tcp_tput.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main() {
    int sock_fd;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(sock_fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("connection failed");
        return -1;
    }

    char buffer[255];
    sprintf(buffer, "hello world!\n");
    send(sock_fd, buffer, strlen(buffer), 0);
    close(sock_fd);
    return 0;
}