#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

// Selective Repeat Client implementation in C. run server first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000
#define WINDOW_SIZE 3
#define DATA 0
#define ACK 1
#define FIN 2

typedef struct Frame {
    char data;
    int type;
    int no;
    int next; // Client uses this to request missing frames
} Frame;

int next_missing_frame(char message[], int size);

int main() {
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

    if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Client side processing...
    int started = 0;
    unsigned int len = sizeof(cliaddr);
    char msg[32] = {0};
    int recieved = 0;
    Frame f;
    while (f.type != FIN) {
        recvfrom(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&servaddr, &len);
        if (f.type == DATA) {
            // Uncomment this section of code to simulate a frame going missing
            // if (!started && f.no == 4) {
            //     printf("Skipping frame 4 to simulate missing frame\n");
            //     started = 1;
            //     continue;
            // }

            if (msg[f.no] != 0) {
                continue; // duplicate frame
            }

            printf("Recieved [%d] containing [%c]. Sending ACK\n", f.no, f.data);
            recieved += 1;

            msg[f.no] = f.data;
            f.type = ACK;
            f.next = next_missing_frame(msg, recieved);
            sendto(sockfd, &f, sizeof(f), 0, (struct sockaddr *)&servaddr, len);
        }
    }

    printf("The server sent: %s\n", msg);
    // Close the socket
    close(sockfd);
}

int next_missing_frame(char message[], int end) {
    // For finding the next frame that needs to be sent
    int i;

    for (i = 0; i < end; i++) {
        if (message[i] == 0) {
            return i;
        }
    }

    return i + WINDOW_SIZE; // If no frame is missing, send the next frame of the window
}