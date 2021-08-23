#ifndef TCP_TPUT
#define TCP_TPUT

#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
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

#define SERVER_ADDR "192.168.2.1"
#define SERVER_PORT 6664

#define NUM_THREADS 8

#define BLK_SIZE (1LL<<26)
#define BLK_CNT (1LL<<11)

#endif