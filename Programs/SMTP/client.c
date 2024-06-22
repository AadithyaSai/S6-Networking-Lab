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

    char cliaddr[64], sendaddr[64], recvaddr[64], data[256];
    char cmd[256];

    // SMTP Logic    
    printf("Enter client email address: ");
    scanf("%s", cliaddr);
    sprintf(cmd, "HELO %s", cliaddr);
    write(sockfd, cmd, sizeof(cmd));

    printf("Enter sender email address: ");
    scanf("%s", sendaddr);
    sprintf(cmd, "MAILFROM %s", sendaddr);
    write(sockfd, cmd, sizeof(cmd));

    printf("Enter recipient email address: ");
    scanf("%s", recvaddr);
    sprintf(cmd, "RCPTTO %s", recvaddr);
    write(sockfd, cmd, sizeof(cmd));

    write(sockfd, "DATA", sizeof("DATA"));
    printf("Enter subject: ");
    scanf("\n%[^\n]", data);
    sprintf(cmd, "SUB: %s", data);
    write(sockfd, cmd, sizeof(cmd));

    printf("Enter message: ");
    scanf("\n%[^\n]", data);
    write(sockfd, data, sizeof(data));

    write(sockfd, ".", sizeof("."));

    write(sockfd, "QUIT", sizeof("QUIT"));


    close(sockfd);
}