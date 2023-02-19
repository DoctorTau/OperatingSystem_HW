
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd, result;
    size_t size;
    const char* message = "Hello, world?";
    int message_size = strlen(message) + 1;
    char name[] = "./bbb.fifo";

    (void)umask(0);

    if (mknod(name, S_IFIFO | 0666, 0) < 0) {
        printf("Can\'t create FIFO \n");
    }

    if ((fd = open(name, O_RDWR)) < 0) {
        printf("Can\'t open FIFO for writting \n");
        exit(-1);
    }

    size = write(fd, message, message_size);
    if (size != message_size) {
        printf("Can\'t write all string to FIFO \n");
        exit(-1);
    }

    int buffer_size = 14;
    char buffer_string[buffer_size];
    size = read(fd, buffer_string, buffer_size);
    if (size < 0) {
        printf("Can\'t read string from FIFO \n");
        exit(-1);
    }

    printf("Reader 2 got string: %s\n", buffer_string);
    if (close(fd) < 0)
        printf("Can\'t close FIFO \n");

    if (close(fd) < 0)
        printf("Can\'t close FIFO \n");

    printf("Writer exit \n");
    return 0;
}