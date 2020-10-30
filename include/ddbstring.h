#ifndef DDBSTRING_H
#define DDBSTRING_H

#include <stdint.h>
#include <string.h>

#include "begin_code.h"

/* A dyamic string structure. */
typedef struct DString {
    char*  ptr;
    size_t length;
    size_t capacity;
} DString;

/* A simple structure to represent a static string. */
typedef struct SString {
    const char* ptr;
    size_t      length;
} SString;

/* String functions */

/* Allocate a dynamic string. Pass in an existing pointer to reallocate it as bigger. */
DString* Ddb_AllocString(DString* ptr, size_t size);

/* Allocate a dynamic string on the condition that it needs allocation. */
DDB_FORCE_INLINE DString* Ddb_AllocStringIfNeeded(DString* ptr, size_t size)
{
    return (! ptr  || ptr->capacity < size) ? Ddb_AllocString(ptr, size) : ptr;
}

/* Sets the string to zero length and also zeroes out the string. Does not free the string. */
void Ddb_ClearString(DString* ptr);

/* Sets the string value to STR. */
void Ddb_SetString(DString* ptr, const char* str);

/* Appends a string to STR. The string is reallocated if it's not big enough to contain STR. */
void Ddb_AppendString(DString* ptr, const char* str);

/*
Ddb_CopyString - Copy a string
    Copies a maximum of MAXLEN characters from SRC into the internal string
    located in DST. DST shall be large enough to hold MAXLEN characters. If
    MAXLEN is negative, Ddb_CopyString copies the entire string in SRC up to the first
    terminating NULL character.
*/
void Ddb_CopyString(DString* dst, const char* src, int maxlen);

/* Frees PTR and the string inside it. PTR is invalid after this call. */
void Ddb_FreeString(DString* ptr);

/* Create static string object. */
SString Ddb_NewSString(const char* str);

/* Returns the hash value of STR. Uses the k = 33 algorithm. */
unsigned long int Ddb_Hash(const char* str);

/* Counts all instances of a character in the string STR. */
size_t Ddb_strchrcnt(const char* str, int c);

#include "close_code.h"

#endif /* DDBSTRING_H */
