#include "tcp_tput.h"

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
        // write(STDOUT_FILENO, buffer, len);
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
        pid_t tid;
        if ((tid=fork()) == 0) {
            sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
            int reuse_addr_value = 1;
            setsockopt(sockfd[i], SOL_SOCKET, SO_REUSEADDR, &reuse_addr_value, 1);

            // struct ifreq ifr;

            // memset(&ifr, 0, sizeof(ifr));
            // snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "enp94s0f0");
            // if (setsockopt(sockfd[i], SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0) {
            //     printf("Error: %s\n", strerror(errno));
            //     return -1;
            // }

            struct sockaddr_in server;
            server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
            server.sin_family = AF_INET;
            server.sin_port = htons(SERVER_PORT + i);

            if (bind(sockfd[i], (struct sockaddr *)&server, sizeof(server)) < 0) {
                puts("bind failed");
                printf("Error: %s\n", strerror(errno));
                return -1;
            }

            puts("Successfully binded");

            listen(sockfd[i], 1);
            char s[20];

            int client_size;
            struct sockaddr_in client_addr;
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
            serve(conn_fd_p);
            return 0;
        }
    }
    
    for (int i=0;i!=NUM_THREADS;++i) {
        int status;
        wait(&status);
    }
}