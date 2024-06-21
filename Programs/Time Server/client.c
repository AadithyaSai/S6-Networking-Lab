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

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) != 0)
    {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    char msg_client[32] = "TIME REQUEST";
    sendto(sockfd, msg_client, sizeof(msg_client), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Request sent to server...\n");

    time_t current_time;
    unsigned int len = sizeof(servaddr);
    recvfrom(sockfd, &current_time, sizeof(current_time), 0, (struct sockaddr *)&servaddr, &len);

    // Convert time_t which is in unix time to string
    char *time_str = asctime(localtime(&current_time));

    printf("Current time is: %s\n", time_str);

    close(sockfd);
}
