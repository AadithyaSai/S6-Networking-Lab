#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8000);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    unsigned int len = sizeof(servaddr);
    if (connect(sockfd, (struct sockaddr *)&servaddr, len))
    {
        printf("Socket connection failed... Is the server running?\n");
        exit(-1);
    }
    printf("Client Connected...\n");
    // TODO: Implement SMTP client logic here
}