#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define FILE_DIR "./files/"
#define CONTENT "777\n"
#define EXIT_ERROR -1
#define EXIT_SUCCESS  0

void exitWithError(char* message){
    perror(message);
    exit(EXIT_ERROR);
}

void writeDigitToString(int digit, char* to_write){
    sprintf(to_write, "%d", digit);    
}

void getFilepath(int digit, char* filepath){
    char filename[1024];
    strcpy(filepath, FILE_DIR);
    writeDigitToString(digit, filename);
    strcat(filepath, filename);
}

int main(int arc, char argv[]){
    int file_depth = 0;

    char previous_filename[1024], current_filename[1024];

    getFilepath(file_depth, previous_filename);

    int fd = open(previous_filename, O_CREAT | O_WRONLY, 0666);
    if(fd < 0){
        exitWithError("Error while creating first file.");
    }

    if(write(fd, CONTENT, sizeof(CONTENT)) < 0){
        close(fd);
        exitWithError("Error while writing to file.");
    }

    close(fd);

    getFilepath(file_depth, previous_filename);

    for(;;){
        ++file_depth;
        writeDigitToString(file_depth - 1, previous_filename); 
        getFilepath(file_depth, current_filename);

        if(symlink(previous_filename, current_filename) < 0){
            exitWithError("Error while creating the symlink");
        }

        printf("Created symlink: %s -> %s\n", current_filename, previous_filename);

        fd = open(current_filename, O_RDONLY);
        if (fd == -1) {
            if (errno == ELOOP) {
                printf("Got an ELOOP.");
                close(fd);
                break;
            }
            
            close(fd);
            exitWithError("Error while opening the file");
        }

        close(fd);

        getFilepath(file_depth, current_filename);
        strcpy(previous_filename, current_filename);
    }

    printf("Current recurtion limit: %d", file_depth - 1);

    return EXIT_SUCCESS;
}