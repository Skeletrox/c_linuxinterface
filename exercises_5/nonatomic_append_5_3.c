#include "../common/tlpi_header.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/// @brief Writes a set of bytes to a file (ASCII chars). Will be used when the
/// file is opened in "append" mode.
/// @param fd File descriptor to write to
/// @param n_bytes The number of bytes to write to the file
void plain_write(int fd, size_t n_bytes) {
    char random_char = 'a' + rand() % 26;
    char buffer[n_bytes];
    for (int i = 0; i < n_bytes; ++i) {
        buffer[i] = random_char;
    }
    buffer[n_bytes - 1] = 'x';
    buffer[n_bytes] = '\0';

    int bytes = write(fd, buffer, n_bytes);
    if (bytes == -1) {
        errExit("Writing to file");
    } else {
        printf("PID %d wrote bytes %d\n", getpid(), bytes);
    }
}

/// @brief Seeks to the end and writes a set of bytes to a file (ASCII chars).
/// Will be used when the file is opened in "write" mode.
/// @param fd File descriptor to write to
/// @param n_bytes The number of bytes to write to the file
void seek_to_end_and_write(int fd, size_t n_bytes) {
    char random_char = 'a' + rand() % 26;
    printf("For PID %d random char is %c\n", getpid(), random_char);
    char buffer[n_bytes];
    for (int i = 0; i < n_bytes; ++i) {
        buffer[i] = random_char;
    }
    buffer[n_bytes] = '\0';
    off_t offset = lseek(fd, 0, SEEK_END);
    printf("Offset for PID %d is %ld\n", getpid(), offset);
    sleep(rand() % 6);
    int bytes = write(fd, buffer, n_bytes);
    if (bytes == -1) {
        errExit("Writing to file");
    } else {
        printf("PID %d wrote bytes %d\n", getpid(), bytes);
    }
}

/// @brief This program demonstrates the need for atomic appends when opening a
/// file in append mode.
/// @param argc The number of arguments. 2 for opening a file in append mode,
/// 3 for opening a file in non-append mode, manually seek to end, and write
/// @param argv the arguments as follows: 
/// @return nothing
int main(int argc, char *argv[]) {
    Boolean append_mode = TRUE;
    char *file_name = NULL;
    size_t n_bytes = 0;
    srand(getpid());

    if (argc != 3 && argc != 4) {
        usageErr("Usage: %s FILE num_bytes [x]", argv[0]);
    }

    if (argc == 4) {
        if (strncmp(argv[3], "x", 1))
            usageErr("Extra option can only be 'x', not %s", argv[3]);
        append_mode = FALSE;
    }

    n_bytes = getLong(argv[2], GN_GT_0, "n_bytes");
    file_name = argv[1];

    int fd;
    if (append_mode) {
        fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            errExit("open");
        }
        plain_write(fd, n_bytes);
    } else {
        fd = open(file_name, O_WRONLY | O_CREAT, 0644);
        if (fd == -1) {
            errExit("open");
        }
        seek_to_end_and_write(fd, n_bytes);
    }
    if (close(fd) == -1) {
        errExit("close");
    }
    // If the 'x' argument is supplied, the lseek() function returns the same
    // offset for each process that seeks it. In case of concurrent writes,
    // the same offset returned leads to overwriting of data.

    // In case of O_APPEND, write() is guaranteed to:
    // 1. Be atomic
    // 2. Seek to the end again while writing data.
    // In such a case, even if lseek() returned a different value, write()
    // guarantees that data is written to EoF and no overwrites happen.
}