#include "tcp_tput.h"

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

char buffer[BLK_SIZE];
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

    puts("Successfully connected server.");

    clock_t start = clock();

    memset(buffer, 'a', sizeof(buffer));
    for (long long i=0;i!=BLK_CNT;++i) {
        if (i*100/BLK_CNT != (i-1)*100/BLK_CNT) {
            printf("%lld%%\n", i*100/BLK_CNT);
        }
        write(sock_fd, buffer, BLK_SIZE);
    }
    puts("Transmission done.");

    read(sock_fd, buffer, 2);
    if (buffer[0]=='O' && buffer[1]=='K') {
        double total_time = 1.0 * (clock() - start) / CLOCKS_PER_SEC;
        double tput = 1.0 * BLK_SIZE * BLK_CNT / total_time;
        puts("ACK received.");
        printf("Throughput: %.3lf Gbps\n", tput*8/(1ll<<30));

    } else {
        puts("ACK error");
    }
    close(sock_fd);
    return 0;
}