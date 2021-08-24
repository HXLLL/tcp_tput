#include "tcp_tput.h"

char buffer[BLK_SIZE];

pthread_t workers[NUM_THREADS];

void *flood(void *conn_fd_p) {
    int conn_fd = *(int*)conn_fd_p;
    memset(buffer, 'v', sizeof(buffer));
    for (long long i=0;i!=BLK_CNT;++i) {
        if (i*100/BLK_CNT != (i-1)*100/BLK_CNT) {
            printf("thread: %d: %lld%%\n", 0, i*100/BLK_CNT);
        }
        write(conn_fd, buffer, BLK_SIZE);
    }
    printf("thread %d: Transmission done.", 0);

    read(conn_fd, buffer, 2);
    if (buffer[0]=='O' && buffer[1]=='K') {
        printf("thread %d: ACK received.\n", 0);
    } else {
        printf("thread %d: ACK error.\n", 0);
    }
    close(conn_fd);
    free(conn_fd_p);
    return NULL;
}

int main() {
    struct timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    int start=tv.tv_sec + tv.tv_nsec/1e9;
    for (int i=0;i!=NUM_THREADS;++i) {
        if (fork()==0) {
            int sock_fd;
            sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    //        struct ifreq ifr;
    //
    //        memset(&ifr, 0, sizeof(ifr));
    //        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "enp94s0f0");
    //        if (setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
    //            return -1;
    //        }

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
            flood(conn_fd);
            return 0;
        }
    }
    for (int i=0;i!=NUM_THREADS;++i) {
        int status;
        wait(&status);
    }
    clock_gettime(CLOCK_MONOTONIC, &tv);
    double cur_time=tv.tv_sec+tv.tv_nsec/1e9;
    double total_time = 1.0 * (cur_time-start);
    printf("total_time: %.3lf\n", total_time);
    double tput = NUM_THREADS * BLK_CNT * BLK_SIZE / total_time;
    printf("Throughput: %.3lf Gbps\n", tput * 8 / (1ll << 30));

    return 0;
}