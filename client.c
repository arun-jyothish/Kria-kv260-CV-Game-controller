/*

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUF_SIZE 1024

int init_comm() {
    int sockfd;
    char buffer[BUF_SIZE];
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("10.42.0.10"); // Server IP address

    // Send message to server
    const char *msg = "Hello, Server!";
    sendto(sockfd, msg, strlen(msg), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Message sent to server\n");

    // Receive acknowledgment from server
    socklen_t len = sizeof(server_addr);
    int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &len);
    buffer[n] = '\0'; // Null-terminate the received string
    printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}
