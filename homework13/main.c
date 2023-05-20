#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_DEPTH 777 // Максимальная глубина рекурсии

int main() {
    char filename[] = "a";
    int depth = 0;
    int opened = 1;

    // Создание директории для файлов
    if (mkdir("files", 0777) == -1) {
        perror("Failed to create directory");
        return 1;
    }

    // Переход в директорию файлов
    if (chdir("files") == -1) {
        perror("Failed to change directory");
        return 1;
    }

    // Создание первоначального файла
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to create file");
        return 1;
    }
    fclose(file);

    // Открытие файлов до достижения максимальной глубины
    while (opened && depth < MAX_DEPTH) {
        char linkname[3] = { 'a' + depth, 'a', '\0' };

        // Создание символьной связи
        if (symlink(filename, linkname) == -1) {
            perror("Failed to create symlink");
            break;
        }

        // Попытка открыть файл
        file = fopen(linkname, "r");
        if (file != NULL) {
            fclose(file);
            depth++;
        } else {
            opened = 0; // Файл не удалось открыть
        }
    }

    printf("Глубина рекурсии: %d\n", depth);

    // Удаление всех созданных файлов и символьных связей
    unlink(filename);
    for (int i = 0; i < depth; i++) {
        char linkname[3] = { 'a' + i, 'a', '\0' };
        unlink(linkname);
    }

    // Возврат в исходную директорию и удаление директории файлов
    chdir("..");
    rmdir("files");

    return 0;
}
