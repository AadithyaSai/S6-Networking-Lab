#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Socket bind failed...\n");
        exit(-1);
    }
    printf("Socket successfully binded..\n");

    if ((listen(sockfd, 255)) != 0)
    {
        printf("Listening failed...\n");
        exit(-1);
    }
    printf("Server listening..\n");

    struct sockaddr_in cli;
    unsigned int len = sizeof(cli);
    int connfd = accept(sockfd, (struct sockaddr *)&cli, &len);

    if (connfd < 0)
    {
        printf("Server accept failed...\n");
        exit(-1);
    }
    printf("Server accepted the client...\n");
    // TODO: Implement SMTP server logic here
}