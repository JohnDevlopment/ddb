#ifndef DDB_H
#define DDB_H

/*
This is the public export header for ddb (Dewpsi Database).
This is a Tcl extension compiled with Tcl stub 8.6. This extension
was created by John Russell.
*/

#include <tcl.h>

#ifdef __cplusplus
    #include <cstdio>
    #include <cstdlib>
    #include <cstdint>
extern "C" {
#else
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
#endif /* defined(__cplusplus) */

#include "begin_code.h"

/*
==================================================
struct: DDB_FileHeader
Represents the file header. These types of files
are marked with a "magic" integer identifier.
==================================================
*/
typedef struct DDB_FileHeader {
    uint32_t id;
    uint64_t numEntries;
    uint64_t blockSize;
    uint64_t numColumns;
    uint64_t columnSize;
    uint64_t offset;
} DDB_FileHeader;

/* Initializes the extension API. Called automatically by Tcl's `load` command. */
DDB_API int Ddb_Init(Tcl_Interp* interp);

#ifdef DDB_DEBUG
    #define DDB_ASSERT(x, ...) if (! (x)) { \
            fprintf(stderr, "DDB assertion '%s' failed in %s on line %d -- ", #x, __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            abort(); \
        }
#else
    #define DDB_ASSERT(x, ...)
#endif

#include "close_code.h"

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif /* DDB_H */
