#include <linux/reboot.h>
#include <stdio.h>
#define __USE_XOPEN
#include <time.h>

void main() {
    unsigned int values[] = {
        LINUX_REBOOT_MAGIC1,
        LINUX_REBOOT_MAGIC2,   /* Linus Torvalds birthday */
        LINUX_REBOOT_MAGIC2A,  /* First daughter's birthday */
        LINUX_REBOOT_MAGIC2B,  /* Second daughter's birthday */
        LINUX_REBOOT_MAGIC2C   /* Third daughter's birthday */
    };

    for (int i = 0; i < 5; ++i) {
        printf("%x\n", values[i]);
    }
    for (int i = 1; i < 5; ++i) {
        struct tm curr_struct;
        char buf[9];
        snprintf(buf, 9, "%x", values[i]);
        strptime(buf, "%d%m%Y", &curr_struct);
        char op[11];
        strftime(op, 11, "%d-%m-%Y", &curr_struct);
        printf("%s\n", op);
    }
    printf("\n---\nhttps://esc.sh/blog/linux-reboot-easter-egg/\n---\n");
}