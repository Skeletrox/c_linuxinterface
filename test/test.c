#include "stdio.h"
#include <gnu/libc-version.h>

int main() {
    printf("Hello world!\n");
    const char* version = gnu_get_libc_version();
    printf("\nLibc version: %s\n", version);
    return 0;
}