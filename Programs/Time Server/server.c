#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVER_PORT 6000
#define CLIENT_PORT 8000

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    char msg_client[32];
    unsigned int len = sizeof(cliaddr);
    printf("Waiting for client...\n");
    recvfrom(sockfd, msg_client, sizeof(msg_client), 0, (struct sockaddr *)&cliaddr, &len);
    printf("Client sent: %s\n", msg_client);

    time_t current_time = time(NULL);
    sendto(sockfd, &current_time, sizeof(current_time), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    printf("Sent current time to client...\n");

    close(sockfd);
}
