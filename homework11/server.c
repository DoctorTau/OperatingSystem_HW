#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>

#include "def.h"

int server_fd;
int sender_fd, receiver_fd;
char buffer[BUFFER_SIZE] = {0};

void closeConnection(void) {
    printf("Stopping server...\n");
    close(sender_fd);
    close(receiver_fd);
    close(server_fd);
}

void handleSignal(int signal) {
    if (signal == SIGINT) {
        printf("Received SIGINT.\n");
        memset(buffer, 0, BUFFER_SIZE);
        closeConnection();
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    (void)signal(SIGINT, handleSignal);

    if (argc < 2) {
        printf("Usage: %s <listen_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse the address (avoid bind error: address already in use)
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // 0.0.0.0
    server_addr.sin_port = htons(atoi(argv[1]));  // cast port to "internet" notation

    // Bind socket to address and port
    if (bind(server_fd, (SA *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    struct sockaddr_in client_addrs[MAX_CLIENTS] = {{0}};
    socklen_t client_sizes[MAX_CLIENTS] = {sizeof(client_addrs[0])};

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int temp_fd;
        if ((temp_fd = accept(server_fd, (SA *)&client_addrs[i], &client_sizes[i])) == -1) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection established with client %d.\n", i + 1);

        // Read client's name
        ssize_t bytes_read;
        if ((bytes_read = read(temp_fd, buffer, BUFFER_SIZE)) == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }
        printf("Client %d's name: %s\n", i + 1, buffer);

        if (strcmp(buffer, "sender") == 0) {
            sender_fd = temp_fd;
        } else if (strcmp(buffer, "receiver") == 0) {
            receiver_fd = temp_fd;
        } else {
            printf("Invalid client name: %s\n", buffer);
            exit(EXIT_FAILURE);
        }
    }

    // Receive messages from clients and forward them to the other client
    ssize_t bytes_read;
    while (1) {
        // Read message from client
        bytes_read = read(sender_fd, buffer, BUFFER_SIZE);
        if (bytes_read == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        // Forward message to the second client
        if (send(receiver_fd, buffer, bytes_read, 0) == -1) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        printf("Message from sender forwarded to receiver.\n");

        // Check for termination message
        if (strcmp(buffer, "The End") == 0) {
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    closeConnection();

    return 0;
}
