#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

const int BUFFER_SIZE = 10000;
key_t sem_key = 777;

int getRandomInt(int begin, int end) {
    return rand() % (end - begin) + begin;
}

int createSem() {
    int semid;

    if ((semid = semget(sem_key++, 2, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t create semaphore set\n");
        exit(-1);
    }

    printf("Semaphore set created with id: %d\n", semid);

    return semid;
}

void deleteSem(int semid) {
    if (semctl(semid, 0, IPC_RMID) < 0) {
        printf("Can\'t delete semaphore set\n");
        exit(-1);
    }
}

struct sembuf setSemOperation(int sem_num, int sem_op, int sem_flg) {
    struct sembuf semaphore_operation;
    semaphore_operation.sem_num = sem_num;
    semaphore_operation.sem_op = sem_op;
    semaphore_operation.sem_flg = sem_flg;
    return semaphore_operation;
}

int main(int argc, char const *argv[]) {
    int writer_sem_id, reader_sem_id, writer_process, io_pipe[2], amount_of_operations;
    char buffer_string[BUFFER_SIZE];
    struct sembuf semaphore_operation;
    size_t size;

    if (argc == 1) {
        amount_of_operations = 10;
    } else {
        amount_of_operations = atoi(argv[1]);
    }

    // Semaphore initialization
    writer_sem_id = createSem();
    reader_sem_id = createSem();

    // Create pipe for reader and writer
    if (pipe(io_pipe) < 0) {
        printf("Can\'t create pipe \n");
        exit(-1);
    }

    // Create child process for writing
    writer_process = fork();

    if (writer_process < 0) {
        printf("Can\'t fork child \n");
        exit(-1);
    } else if (writer_process == 0) {
        sleep(1);
        for (int i = 0; i < amount_of_operations; i++) {
            int random_number = getRandomInt(0, 100);
            sprintf(buffer_string, "%d", random_number);
            size = write(io_pipe[1], buffer_string, strlen(buffer_string) + 1);
            if (size < 0) {
                printf("Can\'t write all string to pipe \n");
                exit(-1);
            }

            // Open reader semaphore
            semaphore_operation = setSemOperation(0, 1, 0);
            if (semop(reader_sem_id, &semaphore_operation, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
            // Close writer semaphore
            semaphore_operation = setSemOperation(0, -1, 0);
            if (semop(writer_sem_id, &semaphore_operation, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
        }

        printf("Writer exit \n");
    } else {
        for (int i = 0; i < amount_of_operations; i++) {
            // Close reader semaphore
            semaphore_operation = setSemOperation(0, -1, 0);
            if (semop(reader_sem_id, &semaphore_operation, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }

            size = read(io_pipe[0], buffer_string, BUFFER_SIZE);
            if (size < 0) {
                printf("Can\'t read string from pipe \n");
                exit(-1);
            }

            printf("Get number: %s\n", buffer_string);

            // Open writer semaphore
            semaphore_operation = setSemOperation(0, 1, 0);
            if (semop(writer_sem_id, &semaphore_operation, 1) < 0) {
                printf("Can\'t wait for condition\n");
                exit(-1);
            }
        }

        deleteSem(writer_sem_id);
        deleteSem(reader_sem_id);
        printf("Semaphore deleted \n");

        printf("Reader exit \n");
    }

    return 0;
}
