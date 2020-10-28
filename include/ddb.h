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

#define DDB_FILE_ID 0x42424470

/*
==================================================
struct: DDB_FileHeader
Represents the file header. These types of files
are marked with a "magic" integer identifier.
==================================================
*/
typedef struct DDB_FileHeader {
    uint32_t id;
    uint32_t numEntries;
    uint32_t blockSize;
    uint32_t numColumns;
    uint32_t offset;
} DDB_FileHeader;

/*
==================================================
struct: DDB_ColumnHeader
Represents the column header. Contains the name
of the column, the maximum length of the data,
and the type of data. The *type* field expects
a value of enum DDB_FieldType.
==================================================
*/
typedef struct DDB_ColumnHeader {
    char     name[32];
    uint32_t length;
    char     type;
} DDB_ColumnHeader;

/*
==================================================
enum: DDB_FieldType
Denotes the type of data kept in a particular
column.
==================================================
*/
typedef enum DDB_FieldType {
    DDB_TYPE_BOOL    = 'B',
    DDB_TYPE_STRING  = 'S',
    DDB_TYPE_INTEGER = 'I',
    DDB_TYPE_DOUBLE  = 'D'
} DDB_FieldType;

/* An empty string. */
DDB_API const char* const DdbEmptyString;

/* Initializes the extension API. Called automatically by Tcl's `load` command. */
DDB_API int Ddb_Init(Tcl_Interp* interp);

/*
DDB_ASSERT - Debug assertion macro
    Aborts execution if the condition COND evaluates to false. The arguments following
    COND are forwarded to fprintf. The second argument is the string argument and any
    other arguments, if present, correspond to said string per the rules of printf.
*/
#ifdef DDB_DEBUG
    #define DDB_ASSERT(cond, ...) if (! (cond)) { \
            fprintf(stderr, "DDB assertion '%s' failed in %s on line %d -- ", #cond, __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            abort(); \
        }
#else
    #define DDB_ASSERT(x, ...)
#endif

/*
DDB_ALLOC - Allocate a block of memory
    This function returns a pointer to a block of memory that's large enough to hold
    COUNT instances of TYPE in continguous memory. If it returns NULL, that means no memory
    block large enough could be found.
*/
#define DDB_ALLOC(type, count) (type *) Ddb_Alloc(sizeof(type), count)

/*
DDB_REALLOC - Reallocate a block of memory
    Reallocate a block of memory using P as the source block. COUNT elements of type TYPE
    are allocated. Memory is carried over from the source block as far as it can fit in the
    new block of data. If the returned pointer points to an area of memory that's larger than
    the old one, the new memory is not initialized.
*/
#define DDB_REALLOC(p, type, count) (type *) Ddb_Realloc(p, sizeof(type), count)

/*
==================================================
struct: DDB_DWord
    Represents the two components of a packed
    32-bit integer.
==================================================
*/
typedef struct DDB_DWord {
    uint16_t hi;
    uint16_t lo;
} DDB_DWord;

/* Casts the expression EXP to the given type TYPE. */
#define DDB_STATIC_CAST(type, exp) ((type)(exp))

/* Packs two 16-bit values a 32-bit value. */
#define DDB_PACK_DWORD(hi, lo) (uint32_t)((uint16_t)(hi) << 16 | (uint16_t)(lo))

/* Retrieve the high word of a 32 bit value. */
#define DDB_HIWORD(dw) ((uint16_t)((dw) >> 16))

/* Retrieve the low word of a 32 bit value. */
#define DDB_LOWORD(dw) ((uint16_t)(dw))

/*
DDB_UNPACK_DWORD - Unpacks a 32-bit value into its components
    RET is a struct of type DDB_DWord. Its 'hi' member is set to the high word
    of DW, and its 'lo' member to the low word.
*/
#define DDB_UNPACK_DWORD(ws, dw) \
    ws.hi = DDB_HIWORD(dw); \
    ws.lo = DDB_LOWORD(dw)

/*
DDB_ARRAY_SIZE - Returns the size of an array
    Returns the number of elements in an array of static duration. This works because, when
    a static array is passed to sizeof, it returns the total size of the array, whereas
    sizeof(array[0]) returns the size of an individual element, which is the size of the underlying
    type.
*/
#define DDB_ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))

#include "close_code.h"

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif /* DDB_H */
