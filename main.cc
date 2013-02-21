#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include "bmh.h"

BMH *bmh = new BMH();
pthread_mutex_t lock;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void con_handler(void *data)
{
    char *buffer = (char *) malloc(256 * sizeof(char)), **ret;
    int newsockfd = *((int *) data), n;

    printf("confirming creation of socket %d\n", newsockfd);

    if (newsockfd < 0) {
        error("ERROR on accept");
    }
    bzero(buffer, 256);
    while (1) {
        n = read(newsockfd, buffer, 255);
        printf("read %d bytes.\n", n);
        if (n < 0) {
            error("ERROR reading from socket");
        } else if (0 == n) {
            printf("connection halted.\n");
            break;
        } else if (n >= 3) {
            printf("message: %s\n", buffer);

            for (int i = 0; i < strlen(buffer); i++) {
                if ('\r' == buffer[i] || '\n' == buffer[i]) {
                    buffer[i] = '\0';
                    break;
                }
            }


            pthread_mutex_lock(&lock);
            ret = bmh->search(buffer, 1000);
            pthread_mutex_unlock(&lock);
            printf("message: %s\n", buffer);
            for (int i = 0; i < 1000; i++) {
                if (NULL != ret[i]) {
                    write(newsockfd, ret[i], strlen(ret[i]));
                }
            }
        }
    }

    free(buffer);
    for (int i = 0; i < 1000; i++) {
        if (NULL != ret[i]) {
            free(ret[i]);
        }
    }
    free(ret);

    printf("connection is over\n");
}

int main(int argc, char **argv)
{
    pthread_t thread;
    int sockfd, *newsockfd, portno, on = 1;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;


    // Initialize BMH
    bmh->load_file(argv[1]);


    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        error("ERROR could not initialize mutex\n");
    }


    // Prepare to listen on socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket\n");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[2]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR binding");
    }
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    while (1) {
        newsockfd = (int *) malloc(sizeof(int));
        *newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        printf("creating socket: %d\n", *newsockfd);
        pthread_create(&thread, NULL, (void* (*)(void*)) &con_handler, (void *) newsockfd);
    }

    close(sockfd);

    return 0;
}
