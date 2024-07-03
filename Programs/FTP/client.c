#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

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
    printf("Client Connected...\n");

    // FTP Client Logic
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("File open failed...\n");
        exit(-1);
    }

    printf("Receiving file from server...\n");
    char data[512];
    while (read(sockfd, data, sizeof(data)) != 0) {
        fprintf(file, "%s", data);
    }
    printf("File received from server...\n");

    fclose(file);
    close(sockfd);
}