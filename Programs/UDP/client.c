#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  // socket related functions
#include <unistd.h>  // read & write

// UDP Client implementation in C. run server first

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

    struct sockaddr_in cliaddr;
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(CLIENT_PORT);
    cliaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    /* htonl and htons are to convert values from host order bytes to network order bytes (Different Endian) */

    // Bind the socket to the given address and port
    if (bind(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }

    struct sockaddr_in servaddr; 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(SERVER_PORT);  
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); 
    // Client side processing...
    char msg_client[32] = "Hello, I am client";
    sendto(sockfd, msg_client, sizeof(msg_client), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
    printf("Message sent to server...\n");
    
    char msg_server[32];
    unsigned int len = sizeof(servaddr);
    recvfrom(sockfd, msg_server, sizeof(msg_server), 0, (struct sockaddr*) &servaddr, &len);
    printf("Server sent: %s\n", msg_server);
    
    // Close the socket
    close(sockfd);
}
