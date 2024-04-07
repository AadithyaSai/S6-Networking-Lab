#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h> 
#include <unistd.h>

// Selective Repeat Client implementation in C. run server first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define DATA 0
#define ACK 1
#define FIN 2
#define NACK 3

typedef struct Frame {
    char data;
    int type;
    int no;
} Frame;

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in cliaddr, servaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    // Timeout for NACK
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Error setting timeout\n");
    }   

    // Client side processing...
    int started = 0;
    unsigned int len = sizeof(cliaddr);
    Frame f;
    int frame_num = 0;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    while (f.type != FIN) {
        if (recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &servaddr, &len) < 0) {
            if (started) {
            printf("No frame [%d] recieved... Sending NACK\n", frame_num);
            f.type = NACK;
            f.no = frame_num;
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &servaddr, len);
            } else {
                printf("Waiting for server...\n");
            }
        }
        else if (f.type != FIN) {
            printf("Recieved [%c]. sending ACK\n", f.data);
            frame_num = f.no;
            f.type = ACK;
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &servaddr, len);
        }
    }
    
    // Close the socket
    close(sockfd);
}
