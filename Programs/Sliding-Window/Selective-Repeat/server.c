#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h> // for setting ARQ time limit
#include <unistd.h>

// Selective Repeat Server implementation in C. Run client first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define WINDOW_SIZE 3
#define DATA 0
#define ACK 1
#define FIN 2

typedef struct Frame {
    char data;
    int type; // data = 0, ack = 1, fin=2
    int no;
    int next; // Client uses this to request missing frames
} Frame;

int sendWindow(int sockfd, struct sockaddr_in *cliaddr, int clilen, int frame_num, char *msg);

int main() {
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

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
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
    Frame f;
    int acked = 0;
    int sent = sendWindow(sockfd, &cliaddr, len, 0, msg);
    while (acked < sent) {
        if (recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&cliaddr, &len) == -1) {
            printf("Timed out... resending window from [%d]...\n", acked);
            sendWindow(sockfd, &cliaddr, len, acked, msg);
        } else {
            if (f.type == ACK) {
                printf("Recieved ACK for frame [%d]\n", f.no);
                acked++;
                // Retransmit lost frame
                if (f.next < sent) {
                    f.type = DATA;
                    f.no = f.next;
                    f.data = msg[f.next];
                    sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&cliaddr, len);
                    printf("Resent frame [%d] containting [%c]\n", f.no, f.data);
                }
                // Send next frame
                if (msg[sent] != '\0') {
                    f.type = DATA;
                    f.no = sent;
                    f.data = msg[sent];
                    sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&cliaddr, len);
                    printf("Sent frame [%d] containting [%c]\n", f.no, f.data);
                    sent++;
                }
            } else {
                printf("Bad Frame!\n");
            }
        }
    }

    f.type = FIN;
    sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&cliaddr, len);

    // Close the socket
    close(sockfd);
}

int sendWindow(int sockfd, struct sockaddr_in *cliaddr, int clilen, int frame_num, char *msg) {
    // Sends whole window and returns number of frames sent
    int i = 0;
    Frame f;
    while (msg[frame_num + i] != '\0' && i < WINDOW_SIZE) {
        f.data = msg[frame_num + i];
        f.type = DATA;
        f.no = frame_num + i;
        sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr *)cliaddr, clilen);
        printf("Sent frame [%d] containting [%c]\n", frame_num + i, f.data);
        i++;
    }

    return i;
}