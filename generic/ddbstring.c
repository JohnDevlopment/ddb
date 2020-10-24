#include "ddb.h"
#include "ddbint.h"
#include "ddbstring.h"
#include "memory.h"

#include <tgmath.h>

DString* Ddb_AllocString(DString* ptr, size_t size)
{
    DString* result = ptr;

    DDB_ASSERT(size > 0, "'size' parameter is zero");

    /* Allocate struct pointer. */
    if (! result)
        result = DDB_ALLOC(DString, 1);

    if (size)
    {
        /* Allocate character array for the first time. */
        if (result->ptr == NULL)
        {
            result->ptr      = DDB_ALLOC(char, size);
            result->length   = 0;
            result->capacity = size;
        }
        else
        {
            size_t oldCapacity = result->capacity;
            if (size > oldCapacity)
            {
                result->ptr = DDB_REALLOC(result->ptr, char, size);
                memset(result->ptr + oldCapacity, 0, size - oldCapacity);
            }
        }
    }

    return result;
}

void Ddb_ClearString(DString* ptr)
{
    DDB_ASSERT(ptr, "Invalid 'ptr' argument, is NULL");

    for (size_t i = 0; i < ptr->length; ++i)
        ptr->ptr[i] = '\0';

    ptr->length = 0;
}

void Ddb_SetString(DString* ptr, const char* str)
{
    DDB_ASSERT(ptr, "Invalid 'ptr' argument, is NULL");
    DDB_ASSERT(str, "Invalid 'str' argument, is NULL");

    Ddb_ClearString(ptr);
    Ddb_AppendString(ptr, str);
}

void Ddb_AppendString(DString* ptr, const char* str)
{
    DDB_ASSERT(ptr, "Invalid 'ptr' argument, is NULL");
    DDB_ASSERT(str, "Invalid 'str' argument, is NULL");

    /* Check to make sure the fields in the struct are correctly formed. */
    DDB_ASSERT(ptr->length < ptr->capacity,
        "ptr->length is >= ptr->capacity, which is not allowed: %lu, %lu\n",
        ptr->length, ptr->capacity);

    if (ptr && str)
    {
        size_t newCapacity;

        /* Initialize string data. */
        if (! ptr->capacity)
        {
            newCapacity = strlen(str) + 6U;
            ptr->ptr = DDB_ALLOC(char, newCapacity);
            strcpy(ptr->ptr, str);
            ptr->length = newCapacity - 1U;
        }
        else
        {
            /* Remaining space in string. */
            size_t ptr_diff = (ptr->capacity - 1) - ptr->length;

            DDB_ASSERT(ptr_diff, "ptr->ptr missing its NULL terminator\n");
            if ((int)(ptr_diff - (strlen(str) + 1U)) < 0)
            {
                /* str length + 1 (NULL terminator) + ptr diff + 1 (NULL terminator) */
                newCapacity = strlen(str) + 6U + ptr->capacity;
                ptr->ptr = DDB_REALLOC(ptr->ptr, char, newCapacity);
            }

            strcat(ptr->ptr, str);
            ptr->length = strlen(ptr->ptr);
        }
    }
}

void Ddb_FreeString(DString* ptr)
{
    DDB_ASSERT(ptr, "Invalid 'ptr' argument");

    if (ptr)
    {
        if (ptr->ptr)
        {
            Ddb_Free(ptr->ptr);
            ptr->ptr = NULL;
        }
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

size_t Ddb_strchrcnt(const char* str, int c)
{
    size_t result = 0;

    while (*str != '\0')
    {
        if (*str == (char) c)
            ++result;
        ++str;
    }

    return result;
}

char* Ddb_DeleteChar(const char* str, int c)
{
    return NULL;
}
