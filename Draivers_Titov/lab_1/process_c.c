#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define CONTENT_BUF_SIZE 256

// IOCTL команды
#define SCULL_IOCTL_GET_SIZE 0
#define SCULL_IOCTL_GET_CONTENT 1

// Функция для безопасного вывода содержимого буфера
void print_buffer_content(const char* buffer_name, const char* content, int content_size) {
    printf("  %s: ", buffer_name);
    
    if (content_size == 0) {
        printf("[ПУСТО]\n");
        return;
    }
    
    printf("[");
    for (int i = 0; i < content_size && i < 50; i++) {
        if (content[i] >= 32 && content[i] <= 126) { // Печатные символы
            putchar(content[i]);
        } else {
            printf("\\x%02x", (unsigned char)content[i]);
        }
    }
    
    if (content_size > 50) {
        printf("...] (%d bytes total)", content_size);
    } else {
        printf("] (%d bytes)", content_size);
    }
    printf("\n");
}

int main() {
    int fd1, fd2;
    int size1, size2;
    int iteration = 0;
    
    printf("=== Buffer Monitor with Content ===\n");
    printf("Time  Buf0  Buf1  Status\n");
    printf("===============================\n");
    
    while (1) {
        // Открываем устройства в неблокирующем режиме
        fd1 = open("/dev/scull_buffer0", O_RDONLY | O_NONBLOCK);
        fd2 = open("/dev/scull_buffer1", O_RDONLY | O_NONBLOCK);
        
        if (fd1 < 0 || fd2 < 0) {
            printf("Error opening devices\n");
            usleep(500000);
            continue;
        }
        
        // Получаем размеры буферов
        ioctl(fd1, SCULL_IOCTL_GET_SIZE, &size1);
        ioctl(fd2, SCULL_IOCTL_GET_SIZE, &size2);
        
        // Определяем статус системы
        char status[30];
        if (size1 == 0 && size2 == 0) {
            strcpy(status, "🔄 System starting");
        } else if (size1 > 0 && size2 > 0) {
            strcpy(status, "✅ Both active");
        } else if (size1 > 0 && size2 == 0) {
            strcpy(status, "📤 A writing, B reading");
        } else if (size1 == 0 && size2 > 0) {
            strcpy(status, "📥 B writing, A reading");
        } else if (size1 == BUFFER_SIZE) {
            strcpy(status, "⏸️  Buffer0 full");
        } else if (size2 == BUFFER_SIZE) {
            strcpy(status, "⏸️  Buffer1 full");
        }
        
        // Выводим основную информацию
        printf("%4d: %4d  %4d  %s\n", 
               iteration, size1, size2, status);
        
        // Получаем и выводим содержимое буферов
        if (size1 > 0) {
            char content1[CONTENT_BUF_SIZE];
            if (ioctl(fd1, SCULL_IOCTL_GET_CONTENT, content1) == 0) {
                print_buffer_content("Buffer0", content1, size1);
            } else {
                printf("  Buffer0: [ERROR reading content]\n");
            }
        }
        
        if (size2 > 0) {
            char content2[CONTENT_BUF_SIZE];
            if (ioctl(fd2, SCULL_IOCTL_GET_CONTENT, content2) == 0) {
                print_buffer_content("Buffer1", content2, size2);
            } else {
                printf("  Buffer1: [ERROR reading content]\n");
            }
        }
        
        // Разделитель между итерациями
        if (size1 > 0 || size2 > 0) {
            printf("\n");
        }
        
        close(fd1);
        close(fd2);
        
        iteration++;
        usleep(500000); // Обновление каждые 0.5 секунды
    }

    return 0;
}