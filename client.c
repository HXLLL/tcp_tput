#include "tcp_tput.h"

#include <net/if.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

char buffer[BLK_SIZE];

pthread_t workers[NUM_THREADS];

void *flood(void *conn_fd_p) {
    int conn_fd = *(int*)conn_fd_p;
    memset(buffer, 'a', sizeof(buffer));
    for (long long i=0;i!=BLK_CNT;++i) {
        if (i*100/BLK_CNT != (i-1)*100/BLK_CNT) {
            printf("thread: %d: %lld%%\n", 0, i*100/BLK_CNT);
        }
        write(conn_fd, buffer, BLK_SIZE);
    }
    printf("thread %d: Transmission done.", 0);

    read(conn_fd, buffer, 2);
    if (buffer[0]=='O' && buffer[1]=='K') {
        printf("thread %d: ACK received.", 0);
    } else {
        printf("thread %d: ACK error", 0);
    }
    close(conn_fd);
    free(conn_fd_p);
    return NULL;
}

int main() {
    clock_t start = clock();
    for (int i=0;i!=NUM_THREADS;++i) {
        int sock_fd;
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);

        struct ifreq ifr;

        memset(&ifr, 0, sizeof(ifr));
        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "enp94s0f0");
        if (setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
            return -1;
        }

        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
        server.sin_family = AF_INET;
        server.sin_port = htons(SERVER_PORT+i);

        if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
            puts("connection failed");
            return -1;
        }

        printf("Thread %d Successfully connected server.", i);

        int *conn_fd = malloc(1);
        *conn_fd = sock_fd;
        pthread_create(&workers[i], NULL, flood, conn_fd);
    }
    for (int i=0;i!=NUM_THREADS;++i) {
        pthread_join(workers[i], NULL);
    }
    double total_time = 1.0 * (clock() - start) / CLOCKS_PER_SEC;
    double tput = NUM_THREADS * BLK_CNT * BLK_SIZE / total_time;
    printf("Throughput: %.3lf Gbps\n", tput * 8 / (1ll << 30));

    return 0;
}