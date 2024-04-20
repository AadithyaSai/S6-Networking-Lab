#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h> // For timeout stuff
#include <time.h>     // For time()
#include <unistd.h>

// First half is identical to TCP Client

int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    unsigned int len = sizeof(servaddr);
    if (connect(sockfd, (struct sockaddr *)&servaddr, len)) {
        printf("Socket connection failed... Is the server running?\n");
        exit(-1);
    }
    printf("Socket connected...\n");

    // Setting timeout for read
    // This block if code is not strictly required
    struct timeval tv;
    tv.tv_sec = 50; // 50ms
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

    // Client side processing... Implementing Leaky Bucket
    char q[32]; // Queue for buffering
    int head = 0;
    int tail = 0;

    char c;
    char chr = 'a';

    time_t prev_time = time(NULL); // Could also be int
    while (chr != '\0') {
        if (read(sockfd, &c, 1) > 0) {
            q[head++] = c;
        }

        // if 1s has elapsed
        if (time(NULL) - prev_time >= 1) {
            prev_time = time(NULL);

            // if queue not empty, pop
            if (head != tail) {
                chr = q[tail++];
                if (chr != '\0') {
                    printf("Recieved [%c]\n", chr);
                }
            }
        }
    }

    // Close the socket
    close(sockfd);
}