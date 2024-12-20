#include <stdarg.h>
#include <string.h>
#include "error_funcs.h"
#include "tlpi_header.h"
#include "ename.c.inc" /* defines ename and MAX_ENAME */

NORETURN static void terminate (Boolean useExit3) {
    char *s;

    // Dump core if EF_DUMPCORE is set and nonempty, else either call
    // exit(3) or _exit(2), based on the boolean supplied as an arg

    s = getenv("EF_DUMPCORE");

    if (s != NULL && *s != '\0') {
        abort();
    } else if (useExit3) {
        exit(EXIT_FAILURE);
    } else {
        _exit(EXIT_FAILURE);
    }
}

static void outputError(Boolean useErr, int err, Boolean flushStdout,
                        const char *format, va_list ap) {

    // The size of the error buffer
    #define BUF_SIZE 500

    char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

    // vsnprintf allows us to use a variable length list (va_list) to the
    // format variable as the arguments
    vsnprintf(userMsg, BUF_SIZE, format, ap);

    // err is an error code returned in errno.h. The actual error is never
    // returned as a readable string / enum. The function below works around
    // this by using the array in ename.c.inc
    if (useErr) {
        snprintf(errText, BUF_SIZE, " [%s %s]", (err > 0 && err <= MAX_ENAME) ?
                ename[err] : "?UNKNOWN?", strerror(err));
    } else {
        snprintf(errText, BUF_SIZE, ":");
    }
    int res = snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);
    if (res < 0) {
        printf("%s", userMsg);
        snprintf(buf, BUF_SIZE, "Could not fit error in buffer,"
                                "error printed to stdout");
    }

    if (flushStdout) {
        fflush (stdout);
    }
    fputs(buf, stderr);
    fflush(stderr);

}

void errMsg(const char *format, ...) {
    va_list argList;
    int savedErrno;

    savedErrno = errno; // in case called functions end up changing it

    // Create a variable argument list after format
    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    // va_end is good practice, cleans up argList.
    va_end(argList);

    errno = savedErrno; // Reassign the error back
}

void errExit(const char *format, ...) {
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}

void err_exit(const char *format, ...) {
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errno, FALSE, format, argList);
    va_end(argList);

    terminate(FALSE);
}

void errExitEN(int errnum, const char *format, ...) {
    va_list argList;

    va_start(argList, format);
    outputError(TRUE, errnum, TRUE, format, argList);
    va_end(argList);

    terminate(FALSE);
}

void fatal(const char *format, ...) {
    va_list argList;

    va_start(argList, format);
    // The onus is on the `fatal()` call to provide the appropriate
    // error format.
    outputError(FALSE, 0, TRUE, format, argList);
    va_end(argList);

    terminate(TRUE);
}

void usageErr(const char *format, ...) {
    va_list argList;

    fflush(stdout); /* flush any stdout */

    fprintf(stderr, "Usage: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    exit(EXIT_FAILURE);
}

void cmdLineErr(const char *format, ...) {
    va_list argList;

    fflush(stdout);

    fprintf(stderr, "Command line usage error: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr);
    exit(EXIT_FAILURE);
}