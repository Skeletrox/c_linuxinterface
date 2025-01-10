#include "../common/tlpi_header.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/**
 * Execute this file as follows for better understanding:
 * $ ./seek_append_5_2 & ./seek_append_5_2
 */
int main() {

    const char* FILE_NAME = "poor_file.fl";
    // srand(time(NULL)) sets the same seed. PID is more random
    srand(getpid());
    int mode = O_APPEND | O_WRONLY | O_CREAT;
    int fd = open(FILE_NAME, mode, 0644);
    sleep(rand() % 4);
    {
        // Iteration 1

        if (fd == -1) {
            errExit("open");
        }
        printf("Writing into file for the first time\n");
        lseek(fd, 0, SEEK_SET);
        const char* FIRST_STRING = "This is the first one, a very long string";
        size_t bytes = write(fd, FIRST_STRING, strlen(FIRST_STRING));
    }
    sleep(rand() % 7);
    {
        // Iteration 2
        printf("Writing into file for the second time\n");
        lseek(fd, 0, SEEK_SET);
        const char* SECOND_STRING = "This is the second one";
        size_t bytes = write(fd, SECOND_STRING, strlen(SECOND_STRING));

        if (bytes == -1) {
            errExit("writing data 2");
        }
    }
    sleep(rand() % 8);
    {
        printf("Writing into file for the third time\n");
        lseek(fd, 0, SEEK_SET);
        const char* THIRD_STRING = "This is third";
        size_t bytes = write(fd, THIRD_STRING, strlen(THIRD_STRING));

        if (bytes == -1) {
            errExit("writing data 3");
        }
    }
    sleep(rand() % 5);
    {
        // Iteration 4
        printf("Writing into file for the fourth time\n");
        lseek(fd, 0, SEEK_SET);
        const char* FOURTH_STRING = "Fourth";
        size_t bytes = write(fd, FOURTH_STRING, strlen(FOURTH_STRING));

        if (bytes == -1) {
            errExit("writing data 4");
        }
    }

    if (close(fd) == -1) {
            errExit("close");
    }
    // append_only always writes to the end of the file descriptor regardless
    // of any seek() that happens. Writes are also atomic, implying that
    // there is no risk of two threads/processes overwriting each other.
}