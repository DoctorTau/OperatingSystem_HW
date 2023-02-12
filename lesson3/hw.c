#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

size_t BUFFER_SIZE = 1024;

// Funclion for reading from file.
void ReadFromFile(char* filename, char* buffer) {
    int fd;
    // Open file for reading.
    if ((fd = open(filename, O_RDONLY)) < 0) {
        printf("Can\'t open file %s", filename);
        exit(-1);
    }
    // Getting file size.
    BUFFER_SIZE = read(fd, buffer, BUFFER_SIZE);
    // Throwing an error if file didn't read.
    if (BUFFER_SIZE < 0) {
        printf("Can\'t read file %s", filename);
        exit(-1);
    }
    // Closing the file. 
    close(fd);
}

// Function for writing to file.
void WriteToFile(char* filename, char* buffer) {
    int fd;
    // Opening the file for writing.
    if ((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
        printf("Can\'t open file %s", filename);
        exit(-1);
    }
    // Writing to file.
    BUFFER_SIZE = write(fd, buffer, BUFFER_SIZE);
    if (BUFFER_SIZE < 0) {
        printf("Can\'t write file %s", filename);
        exit(-1);
    }
    // Closing the file.
    close(fd);
}

int main(int argc, char* argv[]) {
    int fd;
    char string[BUFFER_SIZE];
    ReadFromFile(argv[1], string);
    WriteToFile(argv[2], string);
    return 0;
}