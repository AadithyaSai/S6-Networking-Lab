#include <netinet/in.h>  
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // for setting ARQ time limit
#include <sys/socket.h>  
#include <unistd.h>  

// Stop & Wait Server implementation in C. Run server first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define DATA 0
#define ACK 1
#define FIN 2

typedef struct Frame {
    char data;
    int type;  // ack = 1, fin=2
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

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(SERVER_PORT);  
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // For setting the socket to timeout after 1s (waiting time)
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        printf("Error setting timeout\n");
    }   

    // Server side processing...
    char msg[32];
    unsigned int len = sizeof(cliaddr);
    printf("Enter a message: ");
    scanf("%s", msg);
    int i = 0;
    Frame f;
    int frame_num = 0;
    while (msg[i] != '\0') {
        f.data = msg[i];
        f.type = DATA;
        f.no = frame_num;
        sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, len);
        printf("Sent frame [%d]\n", frame_num);
        if (recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, &len) < 0) {
            printf("Timed out...resending frame [%d]\n", frame_num);
        } else if (f.type == ACK) {
            printf("Recieved ack [%d]\n", frame_num);
            i++;
            frame_num++;
        } else {
            printf("Bad Frame!\n");
        }
    }
    f.type = FIN;
    sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, len);

    // Close the socket
    close(sockfd);
}
