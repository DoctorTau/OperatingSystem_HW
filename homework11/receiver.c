#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "def.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        closeWithError("Usage: %s <server_ip> <server_port>\n");
    }

    // Create TCP socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        closeWithError("socket failed");
    }

    // Set server address and port
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_port = htons(atoi(argv[2]));  // cast port to "internet" notation

    // Get server address from arguments
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        closeWithError("inet_pton failed");
    }

    // Connect to server
    if (connect(client_fd, (SA *)&server_addr, sizeof(server_addr)) < 0) {
        closeWithError("connect failed");
    }

    printf("Connected to server.\n");

    // Send "receiver" status to server
    write(client_fd, "receiver", strlen("receiver"));

    while (1) {
        // Receive the next message from the server
        char buffer[BUFFER_SIZE] = {0};
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes_read < 0) {
            closeWithError("Receive failed");
        } else if (bytes_read == 0) {
            printf("Server closed the connection.\n");
            break;
        }
        printf("%s\n", buffer);
        // Check for termination of communication from server.
        if (!strcmp(buffer, "The End"))
            break;
    }

    printf("Closing client...\n");

    close(client_fd);
    return EXIT_SUCCESS;
}
