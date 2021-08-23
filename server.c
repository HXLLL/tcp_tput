#include "tcp_tput.h"

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
        return -1;
    }

    puts("Successfully binded");
    
    listen(sockfd, 1);

    int cnt = 0;

    struct sockaddr_in client_addr;
    while (1) {
        char s[20]; int client_size;
        inet_ntop(AF_INET, &client_addr, s, sizeof(client_addr));
        int cilentfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&client_size);
        printf("client from %s:%d connected", s, client_addr.sin_port);
    }
}