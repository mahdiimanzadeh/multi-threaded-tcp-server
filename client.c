#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE];

    while (1) {
        int sock = 0;

        // Creating socket file descriptor
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            error("Socket creation error");
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            error("Invalid address or Address not supported");
        }

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            error("Connection failed");
        }

        printf("Enter command (WRITE:<message> or READ): ");
        fgets(command, BUFFER_SIZE, stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove newline character

        // Send command to the server
        send(sock, command, strlen(command), 0);

        // Read response from the server
        int valread = read(sock, buffer, BUFFER_SIZE);
        buffer[valread] = '\0';
        printf("Response from server: %s\n", buffer);

        // Clear buffer for next command
        memset(buffer, 0, BUFFER_SIZE);

        close(sock);
    }

    return 0;
}
