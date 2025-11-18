#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct result {
    long value;
    long seed;
};

int main()
{
    int fd1 = open("/dev/rand", O_RDONLY | O_NONBLOCK);
    if (fd1 == -1) {
        perror("Ошибка доступа к устройству /dev/rand");
        return 1;
    }

    struct result rand_res;

    printf("Генерация случайных чисел из драйвера...\n");
    printf("Подвигайте мышью или понажимайте клавиши для добавления энтропии\n");
    printf("Ctrl+C для выхода\n\n");

    while(1) {
        if (ioctl(fd1, 0, &rand_res)) {
            perror("\nОшибка чтения\n");
            goto _exit;
        }

        printf("Number = %ld; Seed = %ld\n", rand_res.value, rand_res.seed);
        sleep(1);
    }

_exit:
    close(fd1);
    return 0;
}