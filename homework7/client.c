#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#include "message.h"

const char *shared_object = "posix-shar-object";

// Функция для вывода ошибок
void sys_err(char *msg) {
    puts(msg);
    exit(1);
}

// Функция для генерации случайного числа в заданном диапазоне
int getRandomNumber(int min, int max) {
    return min + rand() % (max - min);
}

// Функция для преобразования целого числа в строку
char *intToString(int number) {
    char *string = (char *)malloc(sizeof(char) * 10);
    sprintf(string, "%d", number);
    return string;
}

int main(int argc, char *argv[]) {
    int shared_memory_id, number_to_send, number_of_sends, current_send = 0;
    message_t *message_to_send;
    char *message_content;
    srand(time(NULL));

    // Получаем количество сообщений, которые необходимо отправить
    if (argc == 2) {
        number_of_sends = atoi(argv[1]);
    } else {
        printf("You did not specify a number of sends. Default is 10");
        number_of_sends = 10;
    }

    // Открываем общий объект
    if ((shared_memory_id = shm_open(shared_object, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        sys_err("client: object is already open");
    } else {
        printf("Object is open: name = %s, id = 0x%x\n", shared_object, shared_memory_id);
    }

    // Получаем указатель на сообщение
    message_to_send = (message_t *)mmap(0, sizeof(message_t), PROT_WRITE | PROT_READ, MAP_SHARED,
                                        shared_memory_id, 0);
    if (message_to_send == (message_t *)-1) {
        perror("mmap");
        sys_err("client: can't get shared memory");
    }

    // Отправляем сообщения
    while (1) {
        if (current_send == number_of_sends) {
            // Отправляем сообщение о завершении работы
            message_to_send->type = MSG_TYPE_FINISH;
            strncpy(message_to_send->string, message_content, MAX_STRING);
            break;
        }

        // Генерируем случайное число и преобразуем его в строку
        message_content = intToString(getRandomNumber(0, 1000));
        int len = strlen(message_content);
        // Добавляем символ перевода строки в конец строки
        message_content[len - 1] = '\n';
        printf("Sending: %s \n", message_content);

        // Отправляем сообщение
        message_to_send->type = MSG_TYPE_STRING;
        strncpy(message_to_send->string, message_content, MAX_STRING);

        current_send++;
        sleep(1);
    }

    // Закрываем общий объект
    close(shared_memory_id);
    return 0;
}
