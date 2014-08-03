#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<signal.h>
#include "bmh.h"
#include "suffixarray.h"
#include "testsort.h"

SearchAlg *sa;
pthread_mutex_t lock;
char response[512 * 1024];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

static void catch_sigpipe(int signal)
{
    printf("SIGPIPE thrown");
}

void conHandler(void *data)
{
    unsigned char *buffer = (unsigned char *) malloc(256 * sizeof(unsigned char)), **ret;
    int newsockfd = *((int *) data), n;

    printf("confirming creation of socket %d\n", newsockfd);

    // Handling sigpipe
    if (signal(SIGPIPE, catch_sigpipe) == SIG_ERR) {
        error("Could not set handler to SIGPIPE");
    }

    if (newsockfd < 0) {
        error("ERROR on accept");
    }
    bzero(buffer, 256);
    n = recv(newsockfd, buffer, 255, 0);
    timer t;
    t.start();
    printf("read %d bytes.\n", n);
    if (n < 0) {
        printf("ERROR reading from socket\n");
    } else if (0 == n) {
        printf("connection halted.\n");
    } else if (n >= 3) {
        printf("message: %s\n", buffer);

        for (int i = 0; i < strlen((char *) buffer); i++) {
            if ('\r' == buffer[i] || '\n' == buffer[i]) {
                buffer[i] = '\0';
                break;
            }
        }


        pthread_mutex_lock(&lock);
        ret = sa->search(buffer);
        pthread_mutex_unlock(&lock);
        printf("message: %s\n", buffer);
        strcpy(response, "");
        for (int i = 0; i < SEARCH_LIMIT; i++) {
            if (NULL != ret[i]) {
                strcat(response, (char *) ret[i]);
                strcat(response, "\n");
            }
        }
        if (strcmp("", response) == 0) {
            strcpy(response, "0\n");
        }
        send(newsockfd, response, strlen(response), 0);
        t.end();
        printf("on conHandler %s\n", t.toString());
        for (int i = 0; i < SEARCH_LIMIT; i++) {
            if (NULL != ret[i]) {
                free(ret[i]);
            }
        }
        free(ret);

        close(newsockfd);
    }

    free(buffer);

    printf("connection is over\n");
}

int main(int argc, char **argv)
{
    sa = NULL;
    pthread_t thread;
    int sockfd, *newsockfd, portno, on = 1;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    if (1 == argc) {
        printf("Usage: amuzi_search {bmh|suffixarray} file port\n");
    } else if (2 == argc) {
        TestSort *testSort = new TestSort();
        if (strcmp(argv[1], "test_sort_algs") == 0) {
            printf("Testing sort algorithms.\n");
            testSort->testAlgs();
        }
    } else if (4 == argc){
        // Algorithm
        if (strcmp("bmh", argv[1]) == 0) {
            sa = new BMH();
        } else if (strcmp("suffixarray", argv[1]) == 0) {
            sa = new SuffixArray();
        } else {
            printf("Sorry, that algorithm is not implemented\n");
            return 2;
        }

        // Initialize
        sa->loadFile(argv[2]);


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
        portno = atoi(argv[3]);
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
            pthread_create(&thread, NULL, (void* (*)(void*)) &conHandler, (void *) newsockfd);
        }

        close(sockfd);
    }

    return 0;
}
