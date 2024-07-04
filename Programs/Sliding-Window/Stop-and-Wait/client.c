#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <unistd.h>

// Stop & Wait Client implementation in C. run server first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define DATA 0
#define ACK 1
#define FIN 2

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

    // Client side processing...

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int hasErrored = 0;  // For simulating errors in the network
    unsigned int len = sizeof(cliaddr);
    Frame f;
    while (f.type != FIN) {
        recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &servaddr, &len);
        if (f.type != FIN) {
            // Simulate errors in the network
            if (f.no == 2 && !hasErrored) {
                hasErrored = 1;
                printf("Error in network. Dropping frame\n");
                continue;
            }

            printf("Recieved [%c]. sending ACK\n", f.data);

            f.type = ACK;
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &servaddr, len);
        }
    }
    
    // Close the socket
    close(sockfd);
}
