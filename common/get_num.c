#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "get_num.h"

/**
 * Simple error directive for failing a getnumber call.
 * The following arguments are always expected:
 * fname, msg
 * arg and name are optional args.
 * The output is of the form:
 * {fname} error[ in {name}]: {msg}[    offending text: {arg}\n]
 * Once the message is printed out, the function exits.
 */
static void gnFail(const char *fname, const char *msg, const char *arg,
                   const char *name) {
    fprintf(stderr, "%s error", fname);
    if (name != NULL) {
        fprintf(stderr, " (in %s)", name);
    }
    fprintf(stderr, ": %s\n", msg);
    if (arg != NULL && *arg != '\0') {
        fprintf(stderr, "   offending text:%s\n", arg);
    }
    exit(EXIT_FAILURE);
}

static long getNum(const char *fname, const char *arg, int flags,
                   const char *name) {
    long res;
    char *endptr;
    int base;

    // Error out if no number is supplied
    if (arg == NULL || *arg == '\0' ) {
        gnFail(fname, "null or empty string", arg, name);
    }

    base = (flags & GN_ANY_BASE) ? 0 : (flags & GN_BASE_8) ? 8 :
           (flags & GN_BASE_16) ? 16 : 10;

    errno = 0;

    res = strtol(arg, endptr, base);

    if (errno != 0) {
        gnFail(fname, "strtol() failed", arg, name);
    }

    if (*endptr != '\0') {
        gnFail(fname, "nonnumeric characters", arg, name);
    }

    if ((flags & GN_NONNEG) && res < 0) {
        gnFail(fname, "unexpected negative number", arg, name);
    }
    if ((flags & GN_GT_0) && res <= 0) {
        gnFail(fname, "value should be > 0", arg, name);
    }

    return res;
}


long getLong(const char *arg, int flags, const char *name) {
    return getNum("getLong", arg, flags, name);
}

int getInt(const char *arg, int flags, const char *name) {
    
    long res;

    res = getNum("getInt", arg, flags, name);
    if (res > INT_MAX || res < INT_MIN) {
        gnFail("getInt", "integer out of range", arg, name);
    }

    return (int) res;
}
