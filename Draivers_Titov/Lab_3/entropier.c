#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

void fill(struct timeval* source, struct timeval* destination) {
    destination->tv_sec = source->tv_sec;
    destination->tv_usec = source->tv_usec;
}

int main() {
    struct termios old, newSettings;
    struct timeval start, end;
    long elapsed = 0;
    char c;

    int fd1 = open("/dev/rand", O_RDONLY | O_NONBLOCK);
    if (fd1 == -1) {
        perror("Ошибка доступа к устройству /dev/rand");
        return 1;
    }

    printf("Введите строку для добавления энтропии (Enter для выхода):\n");
    // Сохраняем текущие настройки
    tcgetattr(STDIN_FILENO, &old);
    newSettings = old;

    // Меняем настройки
    newSettings.c_lflag &= ~(ICANON | ECHO);
    newSettings.c_cc[VMIN] = 0;
    newSettings.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

    gettimeofday(&start, NULL);
    fill(&start, &end);

    while(1) {
        if (read(STDIN_FILENO, &c, 1) > 0) {

            gettimeofday(&start, NULL);
            // Вычисление времени в микросекундах
            elapsed = labs((long)(end.tv_usec - start.tv_usec));
            fill(&start, &end);

            if (ioctl(fd1, 1, &elapsed)) {
                perror("\nОшибка добавления энтропии\n");
                goto _exit;
            }

            printf("%c", c);
            fflush(stdout);
            if (c == '\n') {
                goto _exit;
            }
        }
    }

_exit:
    // Восстанавливаем настройки
    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    close(fd1);
    return 0;
}