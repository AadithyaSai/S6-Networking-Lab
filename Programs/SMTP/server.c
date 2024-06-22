#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }
    printf("Socket successfully binded..\n");

    if ((listen(sockfd, 255)) != 0) {
        printf("Listening failed...\n");
        exit(-1);
    }
    printf("Server listening..\n");

    struct sockaddr_in cli;
    unsigned int len = sizeof(cli);
    int connfd = accept(sockfd, (struct sockaddr *)&cli, &len);

    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(-1);
    }

    // SMTP Logic

    char msg[256], *cmd, *data;

    // HELO <client addr>
    do {
        read(connfd, &msg, sizeof(msg));
        data = &msg[0];
        cmd = strsep(&data, " ");
    } while (strcmp(cmd, "HELO") != 0);
    printf("HELO command received from %s\n", data);

    // MAILFROM: <sender email addr>
    do {
        read(connfd, &msg, sizeof(msg));
        data = &msg[0];
        cmd = strsep(&data, " ");
    } while (strcmp(cmd, "MAILFROM") != 0);
    printf("MAIL FROM %s\n", data);

    // RCPTTO: <recipient email addr>
    do {
        read(connfd, &msg, sizeof(msg));
        data = &msg[0];
        cmd = strsep(&data, " ");
    } while (strcmp(cmd, "RCPTTO") != 0);
    printf("RCPT TO %s\n", data);

    // DATA
    do {
        read(connfd, &msg, sizeof(msg));
    } while (strcmp(msg, "DATA") != 0);
    printf("DATA command received\n");

    do {
        read(connfd, &msg, sizeof(msg));
        printf("%s\n", msg);
        fflush(stdout);
    } while (strcmp(msg, ".") != 0);
    printf("End of message\n");

    // QUIT
    do {
        read(connfd, &msg, sizeof(msg));
    } while (strcmp(msg, "QUIT") != 0);
    printf("Client Closing\n");

    close(sockfd);
    close(connfd);
}