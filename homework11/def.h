#define BUFFER_SIZE 1024

#define SA struct sockaddr

#define MAX_CLIENTS 2

void closeWithError(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}