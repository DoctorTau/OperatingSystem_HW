#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#include "message.h"

const char *shared_object = "posix-shar-object";

// Функция для вывода ошибок
void sys_err(char *msg) {
    puts(msg);
    exit(1);
}

int main() {
    int shared_memory_id;
    message_t *message_pointer;
    char message_content[MAX_STRING];

    // Создаем и проверяем объект shared memory
    if ((shared_memory_id = shm_open(shared_object, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        sys_err("server: object is already open");
    } else {
        printf("Object is open: name = %s, id = 0x%x\n", shared_object, shared_memory_id);
    }

    // Устанавливаем размер памяти
    if (ftruncate(shared_memory_id, sizeof(message_t)) == -1) {
        perror("ftruncate");
        sys_err("server: memory sizing error");
        return 1;
    } else {
        printf("Memory size set and = %lu\n", sizeof(message_t));
    }

    // Получаем указатель на объект shared memory
    message_pointer = (message_t *)mmap(0, sizeof(message_t), PROT_WRITE | PROT_READ, MAP_SHARED,
                                        shared_memory_id, 0);
    if (message_pointer == (message_t *)-1) {
        sys_err("server: error getting pointer to shared memory");
    }

    message_pointer->type = MSG_TYPE_EMPTY;

    // Бесконечный цикл для чтения и обработки сообщений
    while (1) {
        // Если получено новое сообщение, то обрабатываем его
        if (message_pointer->type != MSG_TYPE_EMPTY) {
            if (message_pointer->type == MSG_TYPE_STRING) {
                printf("%s\n", message_pointer->string);
            } else if (message_pointer->type == MSG_TYPE_FINISH) {
                break;
            }
            message_pointer->type = MSG_TYPE_EMPTY;  // сообщение обработано
        }
    }

    // Удаляем объект shared memory
    if (shm_unlink(shared_object) == -1) {
        perror("shm_unlink");
        sys_err("server: error getting pointer to shared memory");
    }

    return 0;
}
