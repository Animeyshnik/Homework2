#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_NAME "calc_pipe"

int main() {
    mkfifo(FIFO_NAME, 0666); 

    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) {
        std::cerr << "Ошибка: Не удалось открыть FIFO\n";
        return 1;
    }

    char buffer[16];
    ssize_t bytesRead = read(fifo_fd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Результат: " << buffer << std::endl;
    } else {
        std::cerr << "Ошибка чтения из FIFO\n";
    }

    close(fifo_fd);
    return 0;
}

