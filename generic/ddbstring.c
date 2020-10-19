#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"

#include <tgmath.h>

DString* Ddb_AllocString(DString* ptr, size_t size)
{
    DString* result = ptr;
    size_t oldLength;

    DDB_ASSERT(size > 0, "'size' parameter is zero");

    /* Allocate struct pointer. */
    if (! result)
        result = DDB_ALLOC(DString, 1);

    if (size)
    {
        /* Allocate character array for the first time. */
        if (result->ptr == NULL)
        {
            result->ptr    = DDB_ALLOC(char, size);
            result->length = size;
        }
        else
        {
            oldLength = result->length;
            result->ptr = DDB_REALLOC(result->ptr, char, size);
            if (result->length > oldLength)
                memset(result->ptr + oldLength, 0, result->length - oldLength);
        }
    }

    return result;
}

void Ddb_FreeString(DString* ptr)
{
    DDB_ASSERT(ptr, "Invalid 'ptr' argument");

    if (ptr)
    {
        Ddb_Free(ptr->ptr);
        Ddb_Free(ptr);
    }
}

SString Ddb_NewSString(const char* str)
{
    SString result;
    result.ptr = str;
    result.length = strlen(str);

    return result;
}

unsigned long int Ddb_Hash(const char* str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}
