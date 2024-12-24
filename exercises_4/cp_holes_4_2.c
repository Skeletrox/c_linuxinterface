#include "../common/tlpi_header.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 8192

/** For this exercise, we will need to have two modes:
* 1. Pre-exercise mode where we setup the file with a few holes
* 2. Exercise mode where we copy the file with holes to another
* 
* To that effect, we expect the input to be of the following kinds:
* ./cp_holes_4_2 -p file_1
* ./cp_holes_4_2 file_1 file_2
*/ 


int main(int argc, char *argv[]) { 
    int opt;
    Boolean pe_mode = FALSE; // pe = pre-exercise
    char *file_1 = NULL, *file_2 = NULL;

    if (argc != 3) {
        usageErr("Format: ./cp_holes_4_2 [-p file_1] [file_1 file_2]");
    }

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                pe_mode = TRUE;
                file_1 = optarg;
                if (!file_1) {
                    cmdLineErr("-p requires a file to follow it");
                }
                break;
            default:
                usageErr("-p is the only option defined.");
                break;
        }
    }

    if (pe_mode) {
        // Exit with an error if there are still extra parameters
        if (optind != argc) {
            errExit("Extra parameters in input!");
        }
        // Open the file and write holes into it
        int fd = open(file_1, O_TRUNC | O_WRONLY | O_CREAT, 0644);
        if (fd == -1) {
            errExit("open");
        }

        #define SEEK_LEN 65535
        char garbage[BUF_LEN];
        // Make this garbage have some values
        for (int i = 0 ; i < BUF_LEN; ++i) {
            garbage[i] = 'a';
        }
        garbage[BUF_LEN] = '\0';
        size_t bytes = write(fd, garbage, BUF_LEN);
        if (bytes == -1) {
            errExit("writing initial data");
        }
        lseek(fd, SEEK_LEN, SEEK_CUR);
        bytes = write(fd, garbage, BUF_LEN);
        if (bytes == -1) {
            errExit("writinf data after hole");
        }
        if (close(fd) == -1) {
            errExit("close on initial data");
        }
    } else {
        file_1 = argv[1];
        file_2 = argv[2];
        // Iteratively read through file
        // Count NULL characters as holes (programmatically holes are NULL
        // characters)
        // When we no longer see a hole, dump the value in the file.
        int file_1_fd = open(file_1, O_RDONLY);
        if (file_1_fd == -1) {
            errExit("Opening source file");
        }
        int file_2_fd = open(file_2, O_TRUNC | O_WRONLY | O_CREAT, 0644);
        if (file_2_fd == -1) {
            errExit("Opening target file");
        }
        unsigned int hole_size = 0;
        char buffer[BUF_LEN];
        while(read(file_1_fd, buffer, BUF_LEN) != 0) {
            for (int i = 0; i < BUF_LEN; ++i) {
                // We found a hole. Increment the counter, but do not write
                // this into the destination file.
                if (buffer[i] == '\0') {
                    ++hole_size;
                    continue;
                }
                // This is not a hole. Flush the hole into the file (if any)
                // and then write the actual data
                if (hole_size) {
                    lseek(file_2_fd, hole_size, SEEK_CUR);
                    hole_size = 0;
                }
                if (write(file_2_fd, &buffer[i], 1) != 1) {
                    errExit("Could not write data!");
                }
            }
        };
        if (close(file_1_fd) == -1) {
            errExit("Closing source file");
        }
        if (close(file_2_fd) == -1) {
            errExit("Closing target file");
        }
    }

}