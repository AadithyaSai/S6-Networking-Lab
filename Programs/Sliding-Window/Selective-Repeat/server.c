#include <netinet/in.h>  
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // for setting ARQ time limit
#include <sys/socket.h>  
#include <unistd.h>  

// Selective Repeat Server implementation in C. Run client first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define WINDOW_SIZE 3
#define DATA 0
#define ACK 1
#define FIN 2
#define NACK 3

typedef struct Frame {
    char data;
    int type;  // data = 0, ack = 1, fin=2, nack=3
    int no;
} Frame;

int sendWindow(int sockfd, struct sockaddr_in *cliaddr, int clilen, int frame_num, char *msg);

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

    // Server side processing...
    char msg[32];
    unsigned int len = sizeof(cliaddr);
    printf("Enter a message: ");
    scanf("%s", msg);
    Frame f;
    int i = sendWindow(sockfd, &cliaddr, len, 0, msg);
    while (msg[i] != '\0') {
        recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, &len);
        
        if (f.type == ACK) {
            printf("Recieved ack [%d]\n", f.no);
            f.data = msg[i];
            f.type = DATA;
            f.no = i+WINDOW_SIZE;
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, len);
            printf("Sent frame [%d] containting [%c]\n", i+WINDOW_SIZE, f.data);
            i++;
        } else if (f.type == NACK) {
            printf("Recieved nack [%d]...resending\n", f.no);
            f.data = msg[f.no];
            f.type = DATA;
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, len);
        } else {
            printf("Bad Frame!\n");
        }
    }
    f.type = FIN;
    sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) &cliaddr, len);

    // Close the socket
    close(sockfd);
}

int sendWindow(int sockfd, struct sockaddr_in *cliaddr, int clilen, int frame_num, char *msg) {
    // Sends whole window and returns number of frames sent
    int i = 0;
    Frame f;
    while (msg[frame_num+i] != '\0' && i < WINDOW_SIZE) {
        f.data = msg[frame_num+i];
        f.type = DATA;
        f.no = frame_num + i;
        sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr*) cliaddr, clilen);
        printf("Sent frame [%d] containting [%c]\n", frame_num+i, f.data);
        i++;
    }

    return i;
}