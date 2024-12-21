/*
        CODE FOR Host-PC open socker and receive bbox co-ordinates
        and other attributes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
  printf("Communication .. code\n");

  int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    /* if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) { */
    /*     if (inet_pton(AF_INET, "10.42.0.10", &server_addr.sin_addr) <= 0) { */
    /*       perror("Invalid address or Address not supported"); */
    /*       close(sockfd); */
    /*       exit(EXIT_FAILURE); */
    /*     } */
    /* // Bind the socket to the address */
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(client_addr);
    int n;

    printf("Server listening on port %d...\n", PORT);

    while(1) {
      
    // Receive message from client
    n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &len);
    buffer[n] = '\0'; // Null-terminate the received string
    printf("Client: %s\n", buffer);
    }

    // Send acknowledgment to client
    const char *ack = "Message received";
    sendto(sockfd, ack, strlen(ack), 0, (const struct sockaddr *)&client_addr, len);
    printf("Acknowledgment sent\n");

    close(sockfd);
    return 0;
}
