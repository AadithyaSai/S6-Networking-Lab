#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>  // socket related functions
#include <unistd.h>  // read & write

// TCP Client implementation in C. Run simultaneously with server.c

int main()
{
    // socket initialised. AF_INET is IPv4 domain, SOCK STREAM is for TCP. 3rd arg is protocol which may be left 0 for TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(-1);
    }
    printf("Socket successfully created..\n");

    struct sockaddr_in servaddr; // struct containing socket address details. sockaddr_in for IPv4 family
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(8000);  // port 8000
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // loopback address, ie localhost/127.0.0.1
    /* htonl and htons are to convert values from host order bytes to network order bytes (Different Endian) */

    unsigned int len = sizeof(servaddr);  // may be skipped, instead using sizeof directly in function call
    if (connect(sockfd, (struct sockaddr*)&servaddr, len)) {
        printf("Socket connection failed... Is the server running?\n");
        exit(-1);
    }
    printf("Socket connected...\n");

    // Client side processing... Here we send a number to server for calculating if its a square
    int n;
    printf("Enter a number: ");
    scanf("%d", &n);

    write(sockfd, &n, sizeof(n));

    int sqr;
    read(sockfd, &sqr, sizeof(sqr));

    printf("Square is: %d\n", sqr);

    // Close the socket
    close(sockfd);
}