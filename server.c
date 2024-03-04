#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  // socket related functions
#include <unistd.h>  // read & write

// UDP Server implementation in C. Run server first

#define SERVER_PORT 6000
#define CLIENT_PORT 8000

int main()
{
    // socket initialised. AF_INET is IPv4 domain, SOCK_DRAGM is for UDP. 3rd arg is protocol which may be left 0 for UDP too
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr; // struct containing socket address details. sockaddr_in for IPv4 family
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(SERVER_PORT);  // port 8000
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // loopback address, ie localhost/127.0.0.1
    /* htonl and htons are to convert values from host order bytes to network order bytes (Different Endian) */

    // Bind the socket to the given address and port
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Server side processing...
    char msg_client[32];
    unsigned int len = sizeof(cliaddr);
    printf("Waiting for client...\n");
    recvfrom(sockfd, msg_client, sizeof(msg_client), 0, (struct sockaddr*) &cliaddr, &len);
    printf("Client sent: %s\n", msg_client);

    char msg_server[32] = "Hello, I am server";
    sendto(sockfd, msg_server, sizeof(msg_server), 0, (struct sockaddr*) &cliaddr, sizeof(cliaddr));
    printf("Message sent to client...\n");

    // Close the socket
    close(sockfd);
}
