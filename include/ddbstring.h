#ifndef DDBSTRING_H
#define DDBSTRING_H

#include <stdint.h>
#include <string.h>

#include "begin_code.h"

/* A dyamic string structure. */
typedef struct DString {
    char* ptr;
    int   length;
} DString;

/* A simple structure to represent a static string. */
typedef struct SString {
    const char* ptr;
    int         length;
} SString;

/* String functions */

/* Allocate a dynamic string. Pass in an existing pointer to reallocate it as bigger. */
DString* Ddb_AllocString(DString* ptr, size_t size);

/* Allocate a dynamic string on the condition that it needs allocation. */
DDB_FORCE_INLINE DString* Ddb_AllocStringIfNeeded(DString* ptr, size_t size)
{
    return (! ptr || ptr->length < (int) size) ? Ddb_AllocString(ptr, size) : ptr;
}

/* Free the string and any data inside of it. */
void Ddb_FreeString(DString* ptr);

/* Create static string object. */
SString Ddb_NewSString(const char* str);

/* Returns the hash value of STR. Uses the k = 33 algorithm. */
unsigned long int Ddb_Hash(const char* str);

#include "close_code.h"

#endif /* DDBSTRING_H */
