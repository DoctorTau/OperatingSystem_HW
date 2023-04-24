#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
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
    memset(&server_addr, 0, sizeof(server_addr));
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

    // read input messages from stdin and send to server
    char message[BUFFER_SIZE];

    // Send "sender" status to server
    if (write(client_fd, "sender", strlen("sender")) < 0) {
        closeWithError("write failed");
    }

    while (1) {
        printf("> ");
        if (!fgets(message, BUFFER_SIZE, stdin))
            break;
        message[strcspn(message, "\n")] = '\0';  // end with \0
        // send message to server
        if (write(client_fd, message, strlen(message)) < 0) {
            closeWithError("write failed");
        }
        if (!strcmp(message, "The End"))
            break;
    }

    printf("Closing connection.\n");

    close(client_fd);
    return EXIT_SUCCESS;
}
