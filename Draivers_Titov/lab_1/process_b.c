#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define DEV_READ  "/dev/scull_buffer0"
#define DEV_WRITE "/dev/scull_buffer1"
#define MIN_DELAY 200000   // 0.2 секунды
#define MAX_DELAY 600000   // 0.6 секунды

int main() {
    int fd_read, fd_write;
    char message[BUFFER_SIZE];
    char read_buf[BUFFER_SIZE];
    int write_counter = 0;
    ssize_t ret;
    
    srand(time(NULL) ^ getpid() ^ 12345);

    // ОТКРЫВАЕМ В НЕБЛОКИРУЮЩЕМ РЕЖИМЕ
    fd_read = open(DEV_READ, O_RDONLY | O_NONBLOCK);
    fd_write = open(DEV_WRITE, O_WRONLY | O_NONBLOCK);

    if (fd_read < 0 || fd_write < 0) {
        perror("Failed to open devices");
        exit(EXIT_FAILURE);
    }

    printf("Process B started (PID: %d). Reading from %s, Writing to %s\n", 
           getpid(), DEV_READ, DEV_WRITE);

    int operation_count = 0;
    int consecutive_blocks = 0;
    
    while (1) {
        operation_count++;
        
        // ВЕРОЯТНОСТЬ ЧТЕНИЯ: 60%
        if (rand() % 10 < 6) {
            ret = read(fd_read, read_buf, BUFFER_SIZE - 1);
            if (ret > 0) {
                read_buf[ret] = '\0';
                printf("Process B: ✓ Read %zd bytes: '%s'\n", ret, read_buf);
                consecutive_blocks = 0;
            } else if (ret == -EAGAIN) {
                printf("Process B: ⏸️ Read blocked (buffer empty)\n");
                consecutive_blocks++;
            } else {
                perror("Process B: Read error");
            }
        }

        // ВЕРОЯТНОСТЬ ЗАПИСИ: 60%
        if (rand() % 10 < 6) {
            snprintf(message, BUFFER_SIZE, "B#%d_%d", write_counter++, rand() % 1000);
            ret = write(fd_write, message, strlen(message));
            if (ret > 0) {
                printf("Process B: ✓ Wrote %zd bytes: '%s'\n", ret, message);
                consecutive_blocks = 0;
            } else if (ret == -EAGAIN) {
                printf("Process B: ⏸️ Write blocked (buffer full)\n");
                consecutive_blocks++;
            } else {
                perror("Process B: Write error");
            }
        }

        // АДАПТИВНАЯ ЛОГИКА при множественных блокировках
        if (consecutive_blocks > 4) {
            printf("Process B: 🔄 Adaptive recovery mode\n");
            
            // Пробуем противоположную операцию с большей вероятностью
            if (rand() % 4 == 0) {  // 25% шанс на принудительное действие
                if (rand() % 2 == 0) {
                    printf("Process B: 🚀 Forcing read operation\n");
                    read(fd_read, read_buf, BUFFER_SIZE - 1);
                } else {
                    printf("Process B: 🚀 Forcing write operation\n");
                    snprintf(message, BUFFER_SIZE, "B_forced#%d", write_counter++);
                    write(fd_write, message, strlen(message));
                }
            }
            consecutive_blocks = 0;
        }

        // Периодический статус
        if (operation_count % 15 == 0) {
            printf("Process B: 📊 Status - operations: %d, writes: %d\n", 
                   operation_count, write_counter);
        }

        // Случайная задержка
        usleep(MIN_DELAY + rand() % (MAX_DELAY - MIN_DELAY));
    }

    close(fd_read);
    close(fd_write);
    return 0;
}