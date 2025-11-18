#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 256
#define DEV_WRITE "/dev/scull_buffer0"
#define DEV_READ  "/dev/scull_buffer1"
#define MIN_DELAY 200000   // 0.2 секунды
#define MAX_DELAY 600000   // 0.6 секунды

int main() {
    int fd_write, fd_read;
    char message[BUFFER_SIZE];
    char read_buf[BUFFER_SIZE];
    int write_counter = 0;
    ssize_t ret;
    
    srand(time(NULL) ^ getpid());

    // ОТКРЫВАЕМ В НЕБЛОКИРУЮЩЕМ РЕЖИМЕ
    fd_write = open(DEV_WRITE, O_WRONLY | O_NONBLOCK);
    fd_read = open(DEV_READ, O_RDONLY | O_NONBLOCK);

    if (fd_write < 0 || fd_read < 0) {
        perror("Failed to open devices");
        exit(EXIT_FAILURE);
    }

    printf("Process A started (PID: %d). Writing to %s, Reading from %s\n", 
           getpid(), DEV_WRITE, DEV_READ);

    // НАЧАЛЬНАЯ ЗАПИСЬ для инициализации системы
    printf("Process A: Initial write to start communication...\n");
    snprintf(message, BUFFER_SIZE, "A_init_%d", write_counter++);
    ret = write(fd_write, message, strlen(message));
    if (ret > 0) {
        printf("Process A: ✓ Initial wrote %zd bytes\n", ret);
    } else {
        printf("Process A: ⏸️ Initial write blocked\n");
    }

    int operation_count = 0;
    
    while (1) {
        operation_count++;
        
        // ВЕРОЯТНОСТЬ ЗАПИСИ: 70%
        if (rand() % 10 < 7) {
            snprintf(message, BUFFER_SIZE, "A#%d_%d", write_counter++, rand() % 1000);
            ret = write(fd_write, message, strlen(message));
            if (ret > 0) {
                printf("Process A: ✓ Wrote %zd bytes: '%s'\n", ret, message);
            } else if (ret == -EAGAIN) {
                printf("Process A: ⏸️ Write blocked (buffer full)\n");
                
                // Если не можем писать, увеличиваем вероятность чтения
                printf("Process A: ↻ Increasing read probability due to write block\n");
            } else {
                perror("Process A: Write error");
            }
        }

        // ВЕРОЯТНОСТЬ ЧТЕНИЯ: 50% 
        if (rand() % 2 == 0) {
            ret = read(fd_read, read_buf, BUFFER_SIZE - 1);
            if (ret > 0) {
                read_buf[ret] = '\0';
                printf("Process A: ✓ Read %zd bytes: '%s'\n", ret, read_buf);
            } else if (ret == -EAGAIN) {
                printf("Process A: ⏸️ Read blocked (buffer empty)\n");
                
                // Если не можем читать, пытаемся хотя бы записать в следующей итерации
                printf("Process A: ↻ Will try to write more due to read block\n");
            } else {
                perror("Process A: Read error");
            }
        }

        // АДАПТИВНАЯ ЛОГИКА: если много операций подряд блокируются
        static int consecutive_blocks = 0;
        if (ret == -EAGAIN) {
            consecutive_blocks++;
            if (consecutive_blocks > 3) {
                printf("Process A: 🔄 Adaptive mode - forcing different operation\n");
                // Принудительно выполняем противоположную операцию
                if (rand() % 2 == 0) {
                    snprintf(message, BUFFER_SIZE, "A_forced#%d", write_counter++);
                    write(fd_write, message, strlen(message));
                } else {
                    read(fd_read, read_buf, BUFFER_SIZE - 1);
                }
                consecutive_blocks = 0;
            }
        } else {
            consecutive_blocks = 0;
        }

        // Периодический статус
        if (operation_count % 15 == 0) {
            printf("Process A: 📊 Status - operations: %d, writes: %d\n", 
                   operation_count, write_counter);
        }

        // Случайная задержка
        usleep(MIN_DELAY + rand() % (MAX_DELAY - MIN_DELAY));
    }

    close(fd_write);
    close(fd_read);
    return 0;
}