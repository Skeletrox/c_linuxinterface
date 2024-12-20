#include "../common/tlpi_header.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt; // Stores option returned by getopt()
    Boolean append = FALSE; // Boolean to store if we are appending to a file

    while ((opt = getopt(argc, argv, "a")) != -1) {
        switch (opt) {
            case 'a':
                append = TRUE;
                break;
            case '?':
                cmdLineErr("Wrong option: %c\n", optopt);
                break;
            default:
                usageErr("tee_4_1 <option> <file>\n");
                break;
        }
    }

    if (optind != argc - 1) {
        usageErr("tee_4_1 <option> <file>\n");
    }

    

    printf("optind %d argc %d\n", optind, argc);
    char *filename = argv[argc - 1];
    printf("Append = %d\n", append);
    printf("File = %s\n", filename);

    // Reading from stdin
    
    #define READ_BUFFER_SIZE 127
    char buffer[READ_BUFFER_SIZE + 1];
    int mode = (append ? O_APPEND : O_TRUNC) | O_WRONLY | O_CREAT;
    int fd = open(filename, mode, 0644);
    // Write the same data to a file
    if (fd == -1) {
        errExit("open");
    }
    while (read(STDIN_FILENO, buffer, READ_BUFFER_SIZE) != -1) {
        buffer[READ_BUFFER_SIZE] = '\0';
        printf("%s", buffer);

        size_t bytes = write(fd, buffer, strlen(buffer));
        if (bytes == -1) {
            errExit("write");
        }
    }
    if (close(fd) == -1) {
        errExit("close");
    }
}