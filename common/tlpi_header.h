// Common header for code / exercises in TLPI

#ifndef TLPI_HEADER_H
#define TLPI_HEADER_H  // Prevents accidental double imports

#include <sys/types.h>  // type definitions
#include <stdio.h>  // Standard I/O
// Prototypes of common library functions,
// and EXIT_SUCCESS and EXIT_FAILURE constants
#include <stdlib.h>  
#include <unistd.h>
#include <errno.h> // Declares errno and defines error constants in child includes

/// Local imports come below

#include "get_num.h" // Local numeric handling funcs (getInt(), getLong())

#include "error_funcs.h" // Custom error handling functions

// Define an explicit type for booleans
typedef enum { FALSE, TRUE } Boolean;

// Inline definition of minmax
#define min(m, n) ((m) < (n) ? (m) : (n))
#define max(m, n) ((m) > (n) ? (m) : (n))

#endif