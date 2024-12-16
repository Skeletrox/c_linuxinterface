// Header for storing primitives on how we expect functions to error out

#ifndef ERROR_FUNCS_H
#define ERROR_FUNCS_H


/**
 *  Simple stderr printing directive, uses errno as input.
 */
void errMsg(const char *format, ...);

#ifdef __GNUC__

/* This macro prevents `gcc - Wall` complaining that a non-void function that 
   uses these error handlers to exit might exit without returning a value.
*/
#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN // In non-GNUC cases, this is an empty declaration
#endif  // __GNUC__

/**
 * errMsg + terminate the process through exit(), or through abort() if
 * EF_DUMPCORE is defined.
 */
void errExit(const char *format, ...) NORETURN;

/**
 * errMsg + terminate the process through _exit(), do not flush stdout.
 * _exit() does not flush stdio buffers or invoke exit handlers.
 */
void err_exit(const char *format, ...) NORETURN;

/**
 * Given that errno is defined through a macro that is in turn expanded to a
 * function, it can be inefficient for our use cases. This function instead
 * does this using an explicit integer parameter.
 */
void errExitEN(int errnum, const char *format, ...) NORETURN;

/**
 * Custom handler for fatal. Handles errors from functions that do not set
 * errno.
 */
void fatal(const char *format, ...) NORETURN;

/**
 * Custom handler for usage errors in command line arguments
 */
void usageErr(const char *format, ...) NORETURN;

/**
 * Custom handler for command line errors
 */
void cmdLineErr(const char *format, ...) NORETURN;

#endif  // ERROR_FUNCS_H
