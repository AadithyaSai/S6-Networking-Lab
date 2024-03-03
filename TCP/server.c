#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  // socket related functions
#include <unistd.h>  // read & write

// TCP Client implementation in C. Run simultaneously with client.c

int main()
{
    // socket initialised. AF_INET is IPv4 domain, SOCK STREAM is for TCP. 3rd arg is protocol which may be left 0 for TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr; // struct containing socket address details. sockaddr_in for IPv4 family
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(8000);  // port 8000
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // loopback address, ie localhost/127.0.0.1
    /* htonl and htons are to convert values from host order bytes to network order bytes (Different Endian) */

    // Bind the socket to the given address and port
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Socket bind failed...\n");
        exit(-1);
    }
    printf("Socket successfully binded..\n");

    // Marks socket as a passive socket listening for a cllent's connect()
    if ((listen(sockfd, 255)) != 0) {
        printf("Listening failed...\n");
        exit(-1);
    }
    printf("Server listening..\n");

    struct sockaddr_in cli;
    unsigned int len = sizeof(cli); // Unsigned is not necessary
    int connfd = accept(sockfd, (struct sockaddr*)&cli, &len); // When connection accepted, gives the socket fd of client

    if (connfd < 0) {
        printf("Server accept failed...\n");
        exit(-1);
    }
    printf("Server accepted the client...\n");

    // Server side processing... Here we recieve a number from the client, calculate its square and send it back
    int n;
    read(connfd, &n, sizeof(n));
    
    int sqr = n * n;
    write(connfd, &sqr, sizeof(sqr));

    // Close the socket
    close(sockfd);
}